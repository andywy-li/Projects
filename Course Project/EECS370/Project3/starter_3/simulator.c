/*
 * EECS 370 - Project 3: LC-2K Pipelined Simulator (Solution)
 * Cycle-accurate 5-stage pipeline with explicit forwarding and simple
 * branch prediction (predict-not-taken). No internal RF forwarding,
 * thus an extra WBEND pipeline register is used.
 *
 * Supported ISA (no JALR): add, nor, lw, sw, beq, noop, halt
 *
 * Notes:
 *  - Forward only to EX stage (including store-data path via valB),
 *    priority EX/MEM > MEM/WB > WB/END.
 *  - Stall exactly one cycle for lw-use hazards.
 *  - Resolve branches in MEM; on taken, flush IFID & IDEX to NOOP and set PC.
 *  - Halt when HALT reaches MEMWB.
 *
 * DO NOT modify printState/readMachineCode or their helpers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===================== Machine / ISA defs (given) ===================== */
#define NUMMEMORY 65536
#define NUMREGS 8

#define ADD  0
#define NOR  1
#define LW   2
#define SW   3
#define BEQ  4
#define JALR 5 // not used
#define HALT 6
#define NOOP 7

#define NOOPINSTR (NOOP << 22)

static inline int opcode(int instruction)   { return instruction >> 22; }
static inline int field0(int instruction)   { return (instruction >> 19) & 0x7; }
static inline int field1(int instruction)   { return (instruction >> 16) & 0x7; }
static inline int field2(int instruction)   { return instruction & 0xFFFF; }
static inline int convertNum(int num)       { return num - ((num & (1<<15)) ? (1<<16) : 0); }

/* ========================== Pipeline registers ========================= */
typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int valA;
    int valB;
    int offset;     // sign-extended imm16
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int eq;
    int aluResult;
    int valB;       // for SW store data (after forwarding happens in EX)
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;  // value to be written back to RF (for add/nor/lw)
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;  // previous WB data (to support lack of RF internal fwd)
} WBENDType;

typedef struct stateStruct {
    unsigned int numMemory;
    unsigned int cycles;
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
} stateType;

/* ============================ Prototypes (given) =========================== */
void printState(stateType*);
void printInstruction(int);
void readMachineCode(stateType*, char*);

/* ============================== Main ================================== */
int main(int argc, char *argv[]) {
    static stateType state, newState;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    readMachineCode(&state, argv[1]);

    /* ------------ Initialize architectural and pipeline state ------------ */
    state.cycles = 0;
    state.pc = 0;
    for (int i = 0; i < NUMREGS; ++i) state.reg[i] = 0;

    state.IFID.instr  = NOOPINSTR; state.IFID.pcPlus1  = 0;
    state.IDEX.instr  = NOOPINSTR; state.IDEX.pcPlus1  = 0;
    state.IDEX.valA   = state.IDEX.valB = state.IDEX.offset = 0;
    state.EXMEM.instr = NOOPINSTR; state.EXMEM.branchTarget = 0;
    state.EXMEM.eq    = 0;          state.EXMEM.aluResult = 0; state.EXMEM.valB = 0;
    state.MEMWB.instr = NOOPINSTR;  state.MEMWB.writeData = 0;
    state.WBEND.instr = NOOPINSTR;  state.WBEND.writeData = 0;
    /* ------------------------------ END ---------------------------------- */

    newState = state;

    while (opcode(state.MEMWB.instr) != HALT) {
        printState(&state);
        newState.cycles = state.cycles + 1;  // tick the cycle counter

        /* ============================= IF stage ============================= 
           Default: fetch next instr, PC <- PC+1. This can be overridden by:
           - a stall inserted in ID (freeze PC & IFID), or
           - a taken beq decided in MEM (flush & redirect PC).
        */
        newState.pc = state.pc + 1;
        newState.IFID.instr  = state.instrMem[state.pc];
        newState.IFID.pcPlus1 = state.pc + 1;

        /* ============================= ID stage ============================= 
           Decode & read RF into IDEX. Then detect lw-use hazard and, if found,
           insert a bubble (NOOP) into IDEX and freeze IF & PC for one cycle.
        */
        // Default transfer IFID -> IDEX
        newState.IDEX.instr  = state.IFID.instr;
        newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
        newState.IDEX.valA   = state.reg[field0(state.IFID.instr)];
        newState.IDEX.valB   = state.reg[field1(state.IFID.instr)];
        newState.IDEX.offset = convertNum(field2(state.IFID.instr));

        // Detect lw-use hazard: IDEX (this cycle's EX) has lw, and IFID uses its dest
        int exOp   = opcode(state.IDEX.instr);
        int ifOp   = opcode(state.IFID.instr);
        int lwDest = field1(state.IDEX.instr);  // lw dest is field1

        int ifSrcA = field0(state.IFID.instr);  // regA is always a source for most ops
        int ifSrcB = field1(state.IFID.instr);  // second source for add/nor/sw/beq

        int ifUsesA = (ifOp==ADD || ifOp==NOR || ifOp==LW || ifOp==SW || ifOp==BEQ);
        int ifUsesB = (ifOp==ADD || ifOp==NOR || ifOp==SW || ifOp==BEQ);

        if (exOp == LW &&
            ((ifUsesA && ifSrcA == lwDest) || (ifUsesB && ifSrcB == lwDest))) {
            // Insert bubble into EX (IDEX), freeze IFID & PC
            newState.IDEX.instr   = NOOPINSTR;
            newState.IDEX.pcPlus1 = 0;
            newState.IDEX.valA    = newState.IDEX.valB = newState.IDEX.offset = 0;

            newState.IFID = state.IFID;  // keep the same decoded instr
            newState.pc   = state.pc;    // refetch same PC next cycle
        }

        /* ============================= EX stage ============================= 
           ALU + branch compare. Apply explicit forwarding to valA/valB only
           (including sw's store data path).
        */
        
        /* --------- Forwarding to EX (priority: EX/MEM > MEM/WB > WB/END) --------- */
int idOp    = opcode(state.IDEX.instr);
int srcAreg = field0(state.IDEX.instr);
int srcBreg = field1(state.IDEX.instr);

int fwdA = state.IDEX.valA;
int fwdB = state.IDEX.valB;

/* 1) WB/END —— 最低优先级 */
int wbendOp = opcode(state.WBEND.instr);
if (wbendOp == ADD || wbendOp == NOR) {
    int d = field2(state.WBEND.instr) & 0x7;
    if (d != 0) {  // Don't forward from writes to reg0
        if (d == srcAreg) fwdA = state.WBEND.writeData;
        if (d == srcBreg) fwdB = state.WBEND.writeData;
    }
} else if (wbendOp == LW) {
    int d = field1(state.WBEND.instr) & 0x7;
    if (d != 0) {  // Don't forward from writes to reg0
        if (d == srcAreg) fwdA = state.WBEND.writeData;
        if (d == srcBreg) fwdB = state.WBEND.writeData;
    }
}

/* 2) MEM/WB —— 中等优先级（覆盖 WBEND）*/
int memwbOp = opcode(state.MEMWB.instr);
if (memwbOp == ADD || memwbOp == NOR) {
    int d = field2(state.MEMWB.instr) & 0x7;
    if (d != 0) {  // Don't forward from writes to reg0
        if (d == srcAreg) fwdA = state.MEMWB.writeData;
        if (d == srcBreg) fwdB = state.MEMWB.writeData;
    }
} else if (memwbOp == LW) {
    int d = field1(state.MEMWB.instr) & 0x7;
    if (d != 0) {  // Don't forward from writes to reg0
        if (d == srcAreg) fwdA = state.MEMWB.writeData;
        if (d == srcBreg) fwdB = state.MEMWB.writeData;
    }
}

/* 3) EX/MEM —— 最高优先级（覆盖上面两者）*/
int exmemOp = opcode(state.EXMEM.instr);
if (exmemOp == ADD || exmemOp == NOR) {
    int d = field2(state.EXMEM.instr) & 0x7;
    if (d != 0) {  // Don't forward from writes to reg0
        if (d == srcAreg) fwdA = state.EXMEM.aluResult;
        if (d == srcBreg) fwdB = state.EXMEM.aluResult;
    }
}
/* 注：EX/MEM 对 lw 无可用数据（本周期还在读内存），故不从 EX/MEM 给 lw 前递 */

        /* --------- Execute ALU and branch comparison --------- */
        newState.EXMEM.instr = state.IDEX.instr;
        newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
        newState.EXMEM.eq = (fwdA == fwdB);  // Calculate equality for BEQ
        newState.EXMEM.aluResult = 0;
        newState.EXMEM.valB = fwdB;

        switch (idOp) {
    case ADD:
        newState.EXMEM.aluResult = fwdA + fwdB;
        break;
    case NOR:
        newState.EXMEM.aluResult = ~(fwdA | fwdB);
        break;
    case LW:
    case SW:
        newState.EXMEM.aluResult = fwdA + state.IDEX.offset;
        break;
    case BEQ:
        newState.EXMEM.eq = (fwdA == fwdB);
        break;
    case HALT:
    case NOOP:
    default:
        break;
}


        /* ============================= MEM stage ============================ 
           - sw: write to data memory
           - lw: read from data memory, prepare writeData
           - add/nor: pass ALU result as writeData
           - beq: if taken, redirect PC and flush younger instrs
        */
        newState.MEMWB.instr = state.EXMEM.instr;
        newState.MEMWB.writeData = 0;

        int mOp = opcode(state.EXMEM.instr);
        if (mOp == SW) {
            // memory write uses address = aluResult, data = valB (already forwarded)
            newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.valB;
        } else if (mOp == LW) {
            newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
        } else if (mOp == ADD || mOp == NOR) {
            newState.MEMWB.writeData = state.EXMEM.aluResult;
        } else {
            // beq/halt/noop: writeData is Don't-Care
        }

        // Branch resolution: predict-not-taken; if taken, flush IFID, IDEX, and EXMEM
        if (mOp == BEQ && state.EXMEM.eq) {
            newState.pc = state.EXMEM.branchTarget;
            newState.IFID.instr = NOOPINSTR;
            newState.IFID.pcPlus1 = 0;
            newState.IDEX.instr = NOOPINSTR;
            newState.IDEX.pcPlus1 = 0;
            newState.IDEX.valA = newState.IDEX.valB = newState.IDEX.offset = 0;
            // Also flush EXMEM (instruction that just finished EX stage)
            newState.EXMEM.instr = NOOPINSTR;
            newState.EXMEM.branchTarget = 0;
            newState.EXMEM.eq = 0;
            newState.EXMEM.aluResult = 0;
            newState.EXMEM.valB = 0;
        }
/* ============================== WB stage ============================ */
newState.WBEND.instr     = state.MEMWB.instr;       // 供下一拍前递
newState.WBEND.writeData = state.MEMWB.writeData;

int wOp = opcode(state.MEMWB.instr);  // 只定义一次

switch (wOp) {
    case ADD:
    case NOR: {
        int dest = field2(state.MEMWB.instr) & 0x7;
        if (dest != 0) {  // Protect register 0
            newState.reg[dest] = state.MEMWB.writeData;
        }
        break;
    }
    case LW: {
        int dest = field1(state.MEMWB.instr);
        if (dest != 0) {  // Protect register 0
            newState.reg[dest] = state.MEMWB.writeData;
        }
        break;
    }
    default:
        // beq/sw/noop/halt：不写寄存器
        break;
}

        // (beq/sw/noop/halt: no RF write)

        /* ============================= End of cycle ========================= */
        state = newState;
    }

    printf("Machine halted\n");
    printf("Total of %d cycles executed\n", state.cycles);
    printf("Final state of machine:\n");
    printState(&state);
    return 0;
}

/* ======================= Unmodified helper code ======================= */
const char* opcode_to_str_map[] = {
    "add","nor","lw","sw","beq","jalr","halt","noop"
};

void printInstruction(int instr) {
    const char* instr_opcode_str;
    int instr_opcode = opcode(instr);
    if(ADD <= instr_opcode && instr_opcode <= NOOP) {
        instr_opcode_str = opcode_to_str_map[instr_opcode];
    }

    switch (instr_opcode) {
        case ADD:
        case NOR:
        case LW:
        case SW:
        case BEQ:
            printf("%s %d %d %d", instr_opcode_str, field0(instr), field1(instr), convertNum(field2(instr)));
            break;
        case JALR:
            printf("%s %d %d", instr_opcode_str, field0(instr), field1(instr));
            break;
        case HALT:
        case NOOP:
            printf("%s", instr_opcode_str);
            break;
        default:
            printf(".fill %d", instr);
            return;
    }
}

void printState(stateType *statePtr) {
    printf("\n@@@\n");
    printf("state before cycle %d starts:\n", statePtr->cycles);
    printf("\tpc = %d\n", statePtr->pc);

    printf("\tdata memory:\n");
    for (int i=0; i<statePtr->numMemory; ++i) {
        printf("\t\tdataMem[ %d ] = 0x%08X\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (int i=0; i<NUMREGS; ++i) {
        printf("\t\treg[ %d ] = %d\n", i, statePtr->reg[i]);
    }

    // IF/ID
    printf("\tIF/ID pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->IFID.instr);
    printInstruction(statePtr->IFID.instr);
    printf(" )\n");
    printf("\t\tpcPlus1 = %d", statePtr->IFID.pcPlus1);
    if(opcode(statePtr->IFID.instr) == NOOP){ printf(" (Don't Care)"); }
    printf("\n");

    // ID/EX
    int idexOp = opcode(statePtr->IDEX.instr);
    printf("\tID/EX pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->IDEX.instr);
    printInstruction(statePtr->IDEX.instr);
    printf(" )\n");
    printf("\t\tpcPlus1 = %d", statePtr->IDEX.pcPlus1);
    if(idexOp == NOOP){ printf(" (Don't Care)"); }
    printf("\n");
    printf("\t\tvalA = %d", statePtr->IDEX.valA);
    if (idexOp >= HALT || idexOp < 0) { printf(" (Don't Care)"); }
    printf("\n");
    printf("\t\tvalB = %d", statePtr->IDEX.valB);
    if(idexOp == LW || idexOp > BEQ || idexOp < 0) { printf(" (Don't Care)"); }
    printf("\n");
    printf("\t\toffset = %d", statePtr->IDEX.offset);
    if (idexOp != LW && idexOp != SW && idexOp != BEQ) { printf(" (Don't Care)"); }
    printf("\n");

    // EX/MEM
    int exmemOp = opcode(statePtr->EXMEM.instr);
    printf("\tEX/MEM pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->EXMEM.instr);
    printInstruction(statePtr->EXMEM.instr);
    printf(" )\n");
    printf("\t\tbranchTarget %d", statePtr->EXMEM.branchTarget);
    if (exmemOp != BEQ) { printf(" (Don't Care)"); }
    printf("\n");
    printf("\t\teq ? %s", (statePtr->EXMEM.eq ? "True" : "False"));
    if (exmemOp != BEQ) { printf(" (Don't Care)"); }
    printf("\n");
    printf("\t\taluResult = %d", statePtr->EXMEM.aluResult);
    if (exmemOp > SW || exmemOp < 0) { printf(" (Don't Care)"); }
    printf("\n");
    printf("\t\tvalB = %d", statePtr->EXMEM.valB);
    if (exmemOp != SW) { printf(" (Don't Care)"); }
    printf("\n");

    // MEM/WB
    int memwbOp = opcode(statePtr->MEMWB.instr);
    printf("\tMEM/WB pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->MEMWB.instr);
    printInstruction(statePtr->MEMWB.instr);
    printf(" )\n");
    printf("\t\twriteData = %d", statePtr->MEMWB.writeData);
    if (memwbOp >= SW || memwbOp < 0) { printf(" (Don't Care)"); }
    printf("\n");

    // WB/END
    int wbendOp = opcode(statePtr->WBEND.instr);
    printf("\tWB/END pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->WBEND.instr);
    printInstruction(statePtr->WBEND.instr);
    printf(" )\n");
    printf("\t\twriteData = %d", statePtr->WBEND.writeData);
    if (wbendOp >= SW || wbendOp < 0) { printf(" (Don't Care)"); }
    printf("\n");

    printf("end state\n");
    fflush(stdout);
}

#define MAXLINELENGTH 1000
void readMachineCode(stateType *state, char* filename) {
    char line[MAXLINELENGTH];
    FILE *filePtr = fopen(filename, "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", filename);
        exit(1);
    }

    printf("instruction memory:\n");
    for (state->numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; ++state->numMemory) {
        if (sscanf(line, "%x", state->instrMem+state->numMemory) != 1) {
            printf("error in reading address %d\n", state->numMemory);
            exit(1);
        }
        printf("\tinstrMem[ %d ] = 0x%08X ( ", state->numMemory,
            state->instrMem[state->numMemory]);
        printInstruction(state->dataMem[state->numMemory] = state->instrMem[state->numMemory]);
        printf(" )\n");
    }
}
