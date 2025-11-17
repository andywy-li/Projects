/**
 * Project 2
 * Assembler code fragment for LC-2K
 */

 #include <stdbool.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <string.h>
 
 //Every LC2K file will contain less than 1000 lines of assembly.
 #define MAXLINELENGTH 1000
 #define MAXLINES      1000
 #define MAXSYMS       4096
 
 /**
  * Requires: readAndParse is non-static and unmodified from project 1a. 
  *   inFilePtr and outFilePtr must be opened. 
  *   inFilePtr must be rewound before calling this function.
  * Modifies: outFilePtr
  * Effects: Prints the correct machine code for the input file. After 
  *   reading and parsing through inFilePtr, the pointer is rewound.
  *   Most project 1a error checks are done. No undefined labels of any
  *   type are checked, and these are instead resolved to 0.
 */
 /**
  * This function will be provided in an instructor object file once the
  * project 1a deadline + late days has passed.
 */
 // extern void print_inst_machine_code(FILE *inFilePtr, FILE *outFilePtr);
 
 int readAndParse(FILE *, char *, char *, char *, char *, char *);
 static void checkForBlankLinesInCode(FILE *inFilePtr);
 static inline int isNumber(char *);
 static inline void printHexToFile(FILE *, int);
 
 enum {OP_ADD=0, OP_NOR=1, OP_LW=2, OP_SW=3, OP_BEQ=4, OP_JALR=5, OP_HALT=6, OP_NOOP=7};
 
 // ----- utils -----
 static inline int isAlpha(char c){ return (c>='A'&&c<='Z')||(c>='a'&&c<='z'); }
 static inline int isLower(char c){ return (c>='a'&&c<='z'); }
 static inline int isAlnum(char c){ return isAlpha(c)||(c>='0'&&c<='9'); }
 
 static void die(const char* msg){ exit(1); }
 static void dief(const char* fmt, const char* s){ exit(1); }
 
 static void checkLabelValidOrDie(const char* s){
     if(s[0]=='\0') return;
     int n=(int)strlen(s);
     if(n>6 || !isAlpha(s[0])) dief("error: invalid label '%s'", s);
     for(int i=0;i<n;i++) if(!isAlnum(s[i])) dief("error: invalid label '%s'", s);
 }
 
 // ----- sections -----
 typedef enum {SECT_TEXT=0, SECT_DATA=1, SECT_UNDEF=2} Sect;
 typedef struct { char name[7]; Sect sect; int addr; int defined; int isGlobal; } Sym;
 
 // local label table (not emitted)
 typedef struct { char name[7]; Sect sect; int addr; } Local;
 
 typedef struct { int lineOff; char op[6]; char label[7]; } Reloc; // opcode is "lw"/"sw"/".fill"
 
 // tables
 static int32_t textWords[MAXLINES]; int textCnt=0;
 static int32_t dataWords[MAXLINES]; int dataCnt=0;
 
 static Sym   symtab[MAXSYMS];   int symCnt=0;     // only globals (defined + undefined used)
 static Local localtab[MAXSYMS]; int localCnt=0;   // only locals (defined)
 static Reloc relocs[MAXSYMS*2]; int relocCnt=0;
 
 // ----- helpers for symbols -----
 static int findGlobal(const char* name){
     for(int i=0;i<symCnt;i++) if(!strcmp(symtab[i].name,name)) return i;
     return -1;
 }
 static int addGlobal(const char* name, Sect sect, int addr, int defined){
     if(symCnt>=MAXSYMS) die("error: too many symbols");
     strncpy(symtab[symCnt].name,name,6); symtab[symCnt].name[6]='\0';
     symtab[symCnt].sect = sect;
     symtab[symCnt].addr = addr;
     symtab[symCnt].defined = defined;
     symtab[symCnt].isGlobal = 1;
     return symCnt++;
 }
 static int findLocal(const char* name){
     for(int i=0;i<localCnt;i++) if(!strcmp(localtab[i].name,name)) return i;
     return -1;
 }
 static void addLocalOrDie(const char* name, Sect sect, int addr){
     if(findLocal(name)>=0) dief("error: duplicate label '%s'", name);
     if(localCnt>=MAXSYMS) die("error: too many local labels");
     strncpy(localtab[localCnt].name,name,6); localtab[localCnt].name[6]='\0';
     localtab[localCnt].sect=sect; localtab[localCnt].addr=addr;
     localCnt++;
 }
 
 // P2A FIX: absolute address in *this file* per P1 semantics
 static inline int absAddr_of(Sect sect, int sectOff, int textSize){
     return (sect==SECT_TEXT) ? sectOff : (textSize + sectOff);
 }
 
 static int opcodeOf(const char* op){
     if(!strcmp(op,"add")) return OP_ADD;
     if(!strcmp(op,"nor")) return OP_NOR;
     if(!strcmp(op,"lw"))  return OP_LW;
     if(!strcmp(op,"sw"))  return OP_SW;
     if(!strcmp(op,"beq")) return OP_BEQ;
     if(!strcmp(op,"jalr"))return OP_JALR;
     if(!strcmp(op,"halt"))return OP_HALT;
     if(!strcmp(op,"noop"))return OP_NOOP;
     if(!strcmp(op,".fill"))return -1;
     dief("error: unrecognized opcode '%s'", op);
     return -1;
 }
 static int encR(int op,int rA,int rB,int rd){ return (op<<22)|(rA<<19)|(rB<<16)|(rd&7); }
 static int encI(int op,int rA,int rB,int off){ return (op<<22)|(rA<<19)|(rB<<16)|(off&0xFFFF); }
 static int encJ(int op,int rA,int rB){ return (op<<22)|(rA<<19)|(rB<<16); }
 static int encO(int op){ return (op<<22); }
 static int to16_orDie(int x){
     if(x < -32768 || x > 32767) {exit(1); }
     return x & 0xFFFF;
 }
 static int parseRegOrDie(char* s){
     if(!isNumber(s)) dief("error: non-integer register '%s'", s);
     int r=atoi(s); if(r<0||r>7){ dief("error: register out of range '%s'", s); }
     return r;
 }
 
 // record relocation usage
 static void addReloc(int lineOff, const char* op, const char* label){
     if(relocCnt>=MAXSYMS*2) die("error: too many relocations");
     relocs[relocCnt].lineOff = lineOff;
     strncpy(relocs[relocCnt].op, op, 5); relocs[relocCnt].op[5]='\0';
     strncpy(relocs[relocCnt].label, label, 6); relocs[relocCnt].label[6]='\0';
     relocCnt++;
 }
 
 // ----- Pass 1: count text/data, collect labels (locals + defined globals) -----
 // ----- Pass 1: count text/data, collect labels (locals + defined globals) -----
static void pass1(FILE* in){
    rewind(in);
    char lab[MAXLINELENGTH],op[MAXLINELENGTH],a0[MAXLINELENGTH],a1[MAXLINELENGTH],a2[MAXLINELENGTH];
    int pc=0;           // # of TEXT lines seen so far
    int dcnt=0;         // # of DATA (.fill) lines seen so far
    int inData=0;       // have we entered the DATA section already?

    while(readAndParse(in,lab,op,a0,a1,a2)){
        if(op[0]=='\0') continue;

        int isFill = !strcmp(op,".fill");
        // —— 本行所属的段（关键修复点）——
        Sect thisSect = isFill ? SECT_DATA : (inData ? SECT_DATA : SECT_TEXT);
        int  thisOff  = (thisSect==SECT_TEXT) ? pc : dcnt;

        // 先按“本行段别”记录 label
        if(lab[0]){
            checkLabelValidOrDie(lab);
            if(isLower(lab[0])){ // local
                addLocalOrDie(lab, thisSect, thisOff);
            }else{               // global
                if(findGlobal(lab)>=0) dief("error: duplicate label '%s'", lab);
                addGlobal(lab, thisSect, thisOff, 1);
            }
        }

        // 再做“Text->Data 单向顺序”的检查与计数更新
        if(isFill){
            inData = 1;          // 从这一行开始进入 DATA
            dcnt++;              // 这行是 DATA 的第 dcnt 条
        }else{
            if(inData){
                die("error: instructions cannot appear after .fill directives");
            }
            pc++;                // 这行是 TEXT 的第 pc 条
        }
    }

    textCnt = pc;
    dataCnt = dcnt;
}

 
 // ----- Pass 2: encode, resolve, collect sym/reloc for uses -----
 static void pass2(FILE* in){
     rewind(in);
     char lab[MAXLINELENGTH],op[MAXLINELENGTH],x[MAXLINELENGTH],y[MAXLINELENGTH],z[MAXLINELENGTH];
     int pc=0; int dIdx=0;
     while(readAndParse(in,lab,op,x,y,z)){
         if(op[0]=='\0') continue;
 
         if(!strcmp(op,".fill")){
             int32_t v=0;
             if(isNumber(x)){
                 v = atoi(x);
             }else{
                 checkLabelValidOrDie(x);
                 if(isLower(x[0])){ // local must be defined
                     int li = findLocal(x);
                     if(li<0) dief("error: use of undefined local symbolic address '%s'", x);
                     // P2A FIX: use absolute address in this file (P1 semantics)
                     v = absAddr_of(localtab[li].sect, localtab[li].addr, textCnt);
                 }else{ // GLOBAL
                     int gi = findGlobal(x);
                     if(gi<0){ // first time seeing undefined global (used)
                         gi = addGlobal(x, SECT_UNDEF, 0, 0);
                     }
                     if(symtab[gi].defined){
                         v = absAddr_of(symtab[gi].sect, symtab[gi].addr, textCnt);
                     }else{
                         v = 0; // undefined globals assemble as 0
                     }
                 }
                 // Reloc entry for .fill usage (offset in DATA section)
                 addReloc(dIdx, ".fill", x);
             }
             dataWords[dIdx++] = v;
             continue;
         }
 
         int o = opcodeOf(op);
         if(o==OP_ADD || o==OP_NOR){
             int rA=parseRegOrDie(x), rB=parseRegOrDie(y), rd=parseRegOrDie(z);
             textWords[pc++] = encR(o,rA,rB,rd);
         }else if(o==OP_LW || o==OP_SW){
             int rA=parseRegOrDie(x), rB=parseRegOrDie(y);
             int32_t off=0;
             if(isNumber(z)){
                 off = to16_orDie(atoi(z));
             }else{
                 checkLabelValidOrDie(z);
                 if(isLower(z[0])){
                     int li = findLocal(z);
                     if(li<0) dief("error: use of undefined local symbolic address '%s'", z);
                     // P2A FIX: absolute address value into offset field (P1 behavior)
                     off = to16_orDie(absAddr_of(localtab[li].sect, localtab[li].addr, textCnt));
                 }else{
                     int gi = findGlobal(z);
                     if(gi<0) gi = addGlobal(z, SECT_UNDEF, 0, 0); // undefined global, used
                     off = symtab[gi].defined? to16_orDie(absAddr_of(symtab[gi].sect, symtab[gi].addr, textCnt)) : 0;
                 }
                 // Reloc entry for lw/sw usage (offset in TEXT section)
                 addReloc(pc, (o==OP_LW)?"lw":"sw", z);
             }
             textWords[pc++] = encI(o,rA,rB,off);
         } else if (o==OP_BEQ) {
            int rA = parseRegOrDie(x), rB = parseRegOrDie(y);
            int32_t off = 0;
            if (isNumber(z)) {
                off = to16_orDie(atoi(z));
            } else {
                checkLabelValidOrDie(z);
                int targetAbs = 0;
                if (isLower(z[0])) {
                    int li = findLocal(z);
                    if (li < 0) dief("error: use of undefined local symbolic address '%s'", z);
                    // 目标在本文件 —— 计算“绝对地址”(P1 语义)
                    targetAbs = absAddr_of(localtab[li].sect, localtab[li].addr, textCnt);
                } else {
                    int gi = findGlobal(z);
                    if (gi < 0 || !symtab[gi].defined) {
                        dief("error: beq using undefined symbolic address '%s'", z);
                    }
                    // 目标在本文件的全局标签（已定义）
                    targetAbs = absAddr_of(symtab[gi].sect, symtab[gi].addr, textCnt);
                }
                // beq 偏移：目标绝对地址 - (pc+1)
                off = to16_orDie(targetAbs - (pc + 1));
            }
            textWords[pc++] = encI(o, rA, rB, off);
         }else if(o==OP_JALR){
             int rA=parseRegOrDie(x), rB=parseRegOrDie(y);
             textWords[pc++] = encJ(o,rA,rB);
         }else if(o==OP_HALT || o==OP_NOOP){
             textWords[pc++] = encO(o);
         }else{
             dief("error: unrecognized opcode '%s'", op);
         }
     }
 }
 
 // ----- write object file (exact textual format) -----
 static void writeObject(FILE* out){
    // 先计算好头部数量
    int sCount = symCnt;     // 符号表行数 = 全局已定义 + 未定义但被引用
    int rCount = relocCnt;   // 重定位表行数 = 记录的全部使用（除 beq）

    // 一次性写正确的 Header（不再回写）
    fprintf(out, "%d %d %d %d\n", textCnt, dataCnt, sCount, rCount);

    // Text
    for(int i=0;i<textCnt;i++) printHexToFile(out, textWords[i]);

    // Data
    for(int i=0;i<dataCnt;i++) printHexToFile(out, dataWords[i]);

    // Symbol table（注意这里打印“段内偏移”）
    for(int i=0;i<symCnt;i++){
        char sectChar = symtab[i].defined ? ((symtab[i].sect==SECT_TEXT)?'T':'D') : 'U';
        int  offset   = symtab[i].defined ? symtab[i].addr : 0; // 段内偏移 or 0
        fprintf(out, "%s %c %d\n", symtab[i].name, sectChar, offset);
    }

    // Relocation table（lineOff 必须是段内偏移，你已经做对了：Text用 pc，Data用 dIdx）
    for(int i=0;i<relocCnt;i++){
        fprintf(out, "%d %s %s\n", relocs[i].lineOff, relocs[i].op, relocs[i].label);
    }
}
 
 int main(int argc, char **argv) {
     if(argc!=3){
         exit(1);
     }
     FILE* in = fopen(argv[1], "r");
     if(!in){ exit(1); }
     checkForBlankLinesInCode(in);
 
     // Passes
     pass1(in);
     pass2(in);
 
     FILE* out = fopen(argv[2], "w");
     if(!out){ exit(1); }
 
     writeObject(out);
 
     fclose(in); fclose(out);
     return 0;
 }
 
 /*
 * NOTE: The code defined below is not to be modifed as it is implemented correctly.
 */
 
 // Returns non-zero if the line contains only whitespace.
 int lineIsBlank(char *line) {
     char whitespace[4] = {'\t', '\n', '\r', ' '};
     int nonempty_line = 0;
     for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
         int line_char_is_whitespace = 0;
         for(int whitespace_idx = 0; whitespace_idx < 4; ++whitespace_idx) {
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
 
 // Exits 1 if file contains an empty line anywhere other than at the end of the file.
 // Note calling this function rewinds inFilePtr.
 static void checkForBlankLinesInCode(FILE *inFilePtr) {
     char line[MAXLINELENGTH];
     int blank_line_encountered = 0;
     int address_of_blank_line = 0;
     rewind(inFilePtr);
 
     for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
         // Check for line too long
         if (strlen(line) >= MAXLINELENGTH-1) {
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
               exit(1);
             }
         }
     }
     rewind(inFilePtr);
 }
 
 /*
  * Read and parse a line of the assembly-language file.  Fields are returned
  * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
  * allocated to them).
  *
  * Return values:
  *     0 if reached end of file
  *     1 if all went well
  */
 int readAndParse(FILE *inFilePtr, char *label,
     char *opcode, char *arg0, char *arg1, char *arg2) {
 
     char line[MAXLINELENGTH];
     char *ptr = line;
 
     // delete prior values
     label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
 
     // read the line from the assembly-language file
     if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
         // reached end of file
         return(0);
     }
 
     // check for line too long
     if (strlen(line) >= MAXLINELENGTH-1) {
         printf("error: line too long\n");
         exit(1);
     }
 
     // Ignore blank lines at the end of the file.
     if(lineIsBlank(line)) {
         return 0;
     }
 
     // is there a label?
     ptr = line;
     if (sscanf(ptr, "%[^\t\n ]", label)) {
         // successfully read label; advance pointer over the label */
         ptr += strlen(label);
     }
 
     // Parse the rest of the line.  Would be nice to have real regular expressions, but scanf will suffice.
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
 