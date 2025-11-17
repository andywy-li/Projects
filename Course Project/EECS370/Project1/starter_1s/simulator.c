/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS 

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/

// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct 
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

static inline int convertNum(int32_t);

int 
main(int argc, char **argv)
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(2);
    }


    /* read the entire machine-code file into memory */
    for (state.numMemory=0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
		    if (state.numMemory >= MEMORYSIZE) {
			      fprintf(stderr, "exceeded memory size\n");
			      exit(2);
		    }
		    if (sscanf(line, "%x", state.mem+state.numMemory) != 1) {
			      fprintf(stderr, "error in reading address %d\n", state.numMemory);
			      exit(2);
		    }
		    printf("memory[%d]=0x%08X\n", state.numMemory, state.mem[state.numMemory]);
    }
    /*Initialize state */
    state.pc = 0;
    for (int i = 0; i < NUMREGS; ++i) state.reg[i] = 0;

    int instrCount = 0;
    int running = 1;

    while (running) {
        /* Print state BEFORE executing each instruction */
        printState(&state);


        /* Fetch */
        int instr = state.mem[state.pc];

        /* Decode common fields */
        int opcode = (instr >> 22) & 0x7;        /* bits 24-22 */
        int regA   = (instr >> 19) & 0x7;        /* bits 21-19 */
        int regB   = (instr >> 16) & 0x7;        /* bits 18-16 */
        int dest   =  instr        & 0x7;        /* bits 2-0   (R-type) */
        int off16  =  convertNum(instr & 0xFFFF);/* bits 15-0  (I-type) */

        /* Default next PC is fall-through */
        int nextPC = state.pc + 1;

        /* Execute */
        switch (opcode) {
            case 0: /* add */
                state.reg[dest] = state.reg[regA] + state.reg[regB];
                break;

            case 1: /* nor (bitwise NOR) */
                state.reg[dest] = ~(state.reg[regA] | state.reg[regB]);
                break;

            case 2: /* lw: regB = mem[regA + off16] */
            {
                int addr = state.reg[regA] + off16;
                state.reg[regB] = state.mem[addr];
                break;
            }

            case 3: /* sw: mem[regA + off16] = regB */
            {
                int addr = state.reg[regA] + off16;
                state.mem[addr] = state.reg[regB];
                /* Optionally grow visible memory window */
                if (addr >= state.numMemory && addr < MEMORYSIZE) {
                    state.numMemory = addr + 1;
                }
                break;
            }

            case 4: /* beq: if regA==regB then branch to PC+1+off16 */
                if (state.reg[regA] == state.reg[regB]) {
                    nextPC = state.pc + 1 + off16;
                }
                break;

            case 5: /* jalr: regB = PC+1; PC = regA */
            {
                int ret = state.pc + 1;
                nextPC = state.reg[regA];
                state.reg[regB] = ret;
                break;
            }

            case 6: /* halt */
                running = 0; /* will break loop after updating counters */
                break;

            case 7: /* noop */
                /* do nothing */
                break;

            default:
                fprintf(stderr, "error: illegal opcode %d at PC %d\n", opcode, state.pc);
                exit(2);
        }

        /* Commit PC and increment count */
        state.pc = nextPC;
        instrCount++;
    }

    /* Print state ONCE MORE just before exiting */

    printf("machine halted\n");
    printf("total of %d instructions executed\n", instrCount);
    printf("final state of machine:\n");
    printState(&state);   
    //Your code ends here! 

    return(0);
}

/*
* DO NOT MODIFY ANY OF THE CODE BELOW. 
*/

void printState(stateType *statePtr) {
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] 0x%08X\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
	  for (i=0; i<NUMREGS; i++) {
	      printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	  }
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num) 
{
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

/*
* Write any helper functions that you wish down here. 
*/
