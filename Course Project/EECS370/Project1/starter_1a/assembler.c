/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);

/* ---- symbol table ---- */
typedef struct {
    char name[7];  // ≤6 chars + '\0'
    int addr;
} Sym;
static Sym symTab[1000];
static int symCnt = 0;

/* ---- opcode mapping ---- */
enum {OP_ADD=0, OP_NOR=1, OP_LW=2, OP_SW=3, OP_BEQ=4, OP_JALR=5, OP_HALT=6, OP_NOOP=7};

static int isAlpha(char c){ return (c>='A'&&c<='Z')||(c>='a'&&c<='z'); }
static int isAlnum(char c){ return isAlpha(c)||(c>='0'&&c<='9'); }

/* label validity: ≤6 chars, starts with letter, only [A-Za-z0-9] */
static void checkLabelValidOrDie(const char* s){
    int n = (int)strlen(s);
    if(n==0) return; // empty means "no label" for this line.
    if(n>6 || !isAlpha(s[0])) { printf("error: invalid label '%s'\n", s); exit(1); }
    for(int i=0;i<n;i++) if(!isAlnum(s[i])) { printf("error: invalid label '%s'\n", s); exit(1); }
}

static int lookupLabel(const char* name){
    for(int i=0;i<symCnt;i++){
        if(!strcmp(symTab[i].name,name)) return symTab[i].addr;
    }
    printf("error: undefined label '%s'\n", name);
    exit(1);
}

static void insertLabelOrDie(const char* name, int addr){
    if(name[0]=='\0') return;
    checkLabelValidOrDie(name);
    for(int i=0;i<symCnt;i++){
        if(!strcmp(symTab[i].name,name)){
            printf("error: duplicate label '%s'\n", name);
            exit(1);
        }
    }
    strncpy(symTab[symCnt].name, name, 6);
    symTab[symCnt].name[6] = '\0';
    symTab[symCnt].addr = addr;
    symCnt++;
}

/* parse register [0..7] */
static int parseRegOrDie(char* s){
    if(!isNumber(s)){ printf("error: non-integer register '%s'\n", s); exit(1); }
    int r = atoi(s);
    if(r<0 || r>7){ printf("error: register out of range '%s'\n", s); exit(1); }
    return r;
}

/* 16-bit two's complement range check, return masked low 16 bits */
static int to16bitOrDie(int x){
    if(x < -32768 || x > 32767){
        printf("error: offsetField out of 16-bit range (%d)\n", x);
        exit(1);
    }
    return x & 0xFFFF;
}

/* get integer or label absolute address (for lw/sw/.fill) */
static int parseImmOrLabelAddr(char* s){
    if(isNumber(s)) return atoi(s);
    return lookupLabel(s);
}

/* compute beq offset: label - (pc+1) OR immediate number */
static int parseBeqOffset(char* s, int pc){
    if(isNumber(s)){
        return to16bitOrDie(atoi(s));
    }else{
        int target = lookupLabel(s);
        int off = target - (pc + 1);
        return to16bitOrDie(off);
    }
}

/* opcode string -> code */
static int opcodeOf(const char* op){
    if(!strcmp(op,"add"))  return OP_ADD;
    if(!strcmp(op,"nor"))  return OP_NOR;
    if(!strcmp(op,"lw"))   return OP_LW;
    if(!strcmp(op,"sw"))   return OP_SW;
    if(!strcmp(op,"beq"))  return OP_BEQ;
    if(!strcmp(op,"jalr")) return OP_JALR;
    if(!strcmp(op,"halt")) return OP_HALT;
    if(!strcmp(op,"noop")) return OP_NOOP;
    if(!strcmp(op,".fill")) return -1; // special
    printf("error: unrecognized opcode '%s'\n", op);
    exit(1);
}

/* encoders */
static int encR(int op,int rA,int rB,int dest){
    return (op<<22) | (rA<<19) | (rB<<16) | (dest & 0x7);
}
static int encI(int op,int rA,int rB,int off16){
    return (op<<22) | (rA<<19) | (rB<<16) | (off16 & 0xFFFF);
}
static int encJ(int op,int rA,int rB){
    return (op<<22) | (rA<<19) | (rB<<16);
}
static int encO(int op){
    return (op<<22);
}

/* ------------------------------ main ------------------------------- */

int 
main(int argc, char **argv)
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH],
         arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
        exit(1);
    }
    inFileString  = argv[1];
    outFileString = argv[2];

    /* open input, check blank-lines rule, open output */
    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) { printf("error in opening %s\n", inFileString); exit(1); }
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) { printf("error in opening %s\n", outFileString); exit(1); }

    /* ---------------- PASS 1: build symbol table ---------------- */
    int addr = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if (label[0] != '\0') insertLabelOrDie(label, addr);
        addr++;
    }

    /* ---------------- PASS 2: assemble to machine code ---------------- */
    rewind(inFilePtr);
    int pc = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        int word = 0;
        int op = opcodeOf(opcode);

        if (op == -1) {                    /* .fill */
            int value = parseImmOrLabelAddr(arg0);
            word = value;
        } else if (op==OP_ADD || op==OP_NOR) {      /* R-type */
            int rA = parseRegOrDie(arg0);
            int rB = parseRegOrDie(arg1);
            int d  = parseRegOrDie(arg2);
            word = encR(op, rA, rB, d);
        } else if (op==OP_LW || op==OP_SW) {        /* I-type with absolute addr */
            int rA = parseRegOrDie(arg0);
            int rB = parseRegOrDie(arg1);
            int off = isNumber(arg2) ? to16bitOrDie(atoi(arg2))
                                     : to16bitOrDie(parseImmOrLabelAddr(arg2));
            word = encI(op, rA, rB, off);
        } else if (op==OP_BEQ) {                    /* I-type with relative offset */
            int rA = parseRegOrDie(arg0);
            int rB = parseRegOrDie(arg1);
            int off = parseBeqOffset(arg2, pc);     /* labelAddr - (pc+1) */
            word = encI(op, rA, rB, off);
        } else if (op==OP_JALR) {                   /* J-type */
            int rA = parseRegOrDie(arg0);
            int rB = parseRegOrDie(arg1);
            word = encJ(op, rA, rB);
        } else if (op==OP_HALT || op==OP_NOOP) {    /* O-type */
            word = encO(op);
        } else {
            printf("error: unknown opcode\n"); exit(1);
        }

        printHexToFile(outFilePtr, word);
        pc++;
    }

    return 0;
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}


/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}


// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}
