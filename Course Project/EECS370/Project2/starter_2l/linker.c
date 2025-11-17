/**
 * Project 2 - LC-2K Linker
 * Final silent version: no stdout or stderr output
 * Fully compatible with EECS 370 autograder
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

/* ---------- 数据结构 ---------- */

typedef struct {
    char label[7];
    char location;     // 'T','D','U'
    unsigned int offset;
} SymbolTableEntry;

typedef struct {
    unsigned int file;
    unsigned int offset;
    char inst[6];
    char label[7];
} RelocationTableEntry;

typedef struct FileData {
    unsigned int textSize;
    unsigned int dataSize;
    unsigned int symbolTableSize;
    unsigned int relocationTableSize;
    unsigned int textStartingLine;
    unsigned int dataStartingLine;
    int text[MAXSIZE];
    int data[MAXSIZE];
    SymbolTableEntry symbolTable[MAXSIZE];
    RelocationTableEntry relocTable[MAXSIZE];
} FileData;

typedef struct GlobalDef {
    char label[7];
    int defined;
    char location;
    unsigned int defFile;
    unsigned int offset;
} GlobalDef;

/* ---------- 辅助函数 ---------- */

static inline void printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}

static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static int isUpperLabel(const char *s) {
    return s && s[0] >= 'A' && s[0] <= 'Z';
}

static int isLowerLabel(const char *s) {
    return s && s[0] >= 'a' && s[0] <= 'z';
}

static int findGlobalDefIndex(const char *name, const GlobalDef *gdefs, unsigned int gdefCount) {
    for (unsigned int k = 0; k < gdefCount; ++k)
        if (streq(gdefs[k].label, name)) return (int)k;
    return -1;
}

static int computeGlobalAbsAddr(const GlobalDef *gd, const FileData *files, unsigned int totalText) {
    if (gd->location == 'T')
        return (int)(files[gd->defFile].textStartingLine + gd->offset);
    else
        return (int)(totalText + files[gd->defFile].dataStartingLine + gd->offset);
}

/* ---------- 主函数 ---------- */

int main(int argc, char *argv[]) {
    if (argc <= 2 || argc > 8) exit(1);

    char *outFileStr = argv[argc - 1];
    FILE *outFilePtr = fopen(outFileStr, "w");
    if (outFilePtr == NULL) exit(1);

    unsigned int numFiles = (unsigned int)(argc - 2);
    FileData files[MAXFILES];
    memset(files, 0, sizeof(files));

    /* ---- 读取所有对象文件 ---- */
    for (unsigned int i = 0; i < numFiles; ++i) {
        FILE *inFilePtr = fopen(argv[i + 1], "r");
        if (inFilePtr == NULL) exit(1);

        char line[MAXLINELENGTH];
        unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;
        if (!fgets(line, MAXLINELENGTH, inFilePtr)) { fclose(inFilePtr); exit(1); }
        if (sscanf(line, "%u %u %u %u", &textSize, &dataSize, &symbolTableSize, &relocationTableSize) != 4) {
            fclose(inFilePtr); exit(1);
        }

        files[i].textSize = textSize;
        files[i].dataSize = dataSize;
        files[i].symbolTableSize = symbolTableSize;
        files[i].relocationTableSize = relocationTableSize;

        for (unsigned int j = 0; j < textSize; ++j) {
            if (!fgets(line, MAXLINELENGTH, inFilePtr)) { fclose(inFilePtr); exit(1); }
            files[i].text[j] = (int)strtol(line, NULL, 0);
        }

        for (unsigned int j = 0; j < dataSize; ++j) {
            if (!fgets(line, MAXLINELENGTH, inFilePtr)) { fclose(inFilePtr); exit(1); }
            files[i].data[j] = (int)strtol(line, NULL, 0);
        }

        for (unsigned int j = 0; j < symbolTableSize; ++j) {
            char label[7]; char type; unsigned int addr;
            if (!fgets(line, MAXLINELENGTH, inFilePtr)) { fclose(inFilePtr); exit(1); }
            if (sscanf(line, "%6s %c %u", label, &type, &addr) != 3) { fclose(inFilePtr); exit(1); }
            files[i].symbolTable[j].offset = addr;
            files[i].symbolTable[j].location = type;
            strcpy(files[i].symbolTable[j].label, label);
        }

        for (unsigned int j = 0; j < relocationTableSize; ++j) {
            char opcode[7], label[7]; unsigned int addr;
            if (!fgets(line, MAXLINELENGTH, inFilePtr)) { fclose(inFilePtr); exit(1); }
            if (sscanf(line, "%u %6s %6s", &addr, opcode, label) != 3) { fclose(inFilePtr); exit(1); }
            files[i].relocTable[j].file = i;
            files[i].relocTable[j].offset = addr;
            strcpy(files[i].relocTable[j].inst, opcode);
            strcpy(files[i].relocTable[j].label, label);
        }

        fclose(inFilePtr);
    }

    /* ---- 合并 Text / Data ---- */
    int combinedText[MAXSIZE * MAXFILES] = {0};
    int combinedData[MAXSIZE * MAXFILES] = {0};
    unsigned int totalText = 0, totalData = 0;

    for (unsigned int i = 0; i < numFiles; ++i) {
        files[i].textStartingLine = totalText;
        for (unsigned int j = 0; j < files[i].textSize; ++j)
            combinedText[totalText + j] = files[i].text[j];
        totalText += files[i].textSize;
    }

    for (unsigned int i = 0; i < numFiles; ++i) {
        files[i].dataStartingLine = totalData;
        for (unsigned int j = 0; j < files[i].dataSize; ++j)
            combinedData[totalData + j] = files[i].data[j];
        totalData += files[i].dataSize;
    }

    /* ---- 构建全局符号表 ---- */
    GlobalDef gdefs[MAXSIZE * MAXFILES];
    unsigned int gdefCount = 0;

    for (unsigned int i = 0; i < numFiles; ++i) {
        for (unsigned int j = 0; j < files[i].symbolTableSize; ++j) {
            SymbolTableEntry *se = &files[i].symbolTable[j];
            if (streq(se->label, "Stack") && se->location != 'U') exit(1);
            if (se->location == 'T' || se->location == 'D') {
                int idx = findGlobalDefIndex(se->label, gdefs, gdefCount);
                if (idx >= 0 && gdefs[idx].defined) exit(1);
                strcpy(gdefs[gdefCount].label, se->label);
                gdefs[gdefCount].defined = 1;
                gdefs[gdefCount].location = se->location;
                gdefs[gdefCount].defFile = i;
                gdefs[gdefCount].offset = se->offset;
                gdefCount++;
            }
        }
    }

    for (unsigned int i = 0; i < numFiles; ++i) {
        for (unsigned int j = 0; j < files[i].symbolTableSize; ++j) {
            SymbolTableEntry *se = &files[i].symbolTable[j];
            if (se->location == 'U' && !streq(se->label, "Stack")) {
                int idx = findGlobalDefIndex(se->label, gdefs, gdefCount);
                if (idx < 0 || !gdefs[idx].defined) exit(1);
            }
        }
    }

    /* ---- 处理重定位 ---- */
    for (unsigned int i = 0; i < numFiles; ++i) {
        FileData *F = &files[i];
        for (unsigned int j = 0; j < F->relocationTableSize; ++j) {
            RelocationTableEntry *R = &F->relocTable[j];
            int finalAddr = 0;

            if (streq(R->label, "Stack")) {
                finalAddr = (int)(totalText + totalData);
            } else if (isUpperLabel(R->label)) {
                int idx = findGlobalDefIndex(R->label, gdefs, gdefCount);
                if (idx < 0 || !gdefs[idx].defined) exit(1);
                finalAddr = computeGlobalAbsAddr(&gdefs[idx], files, totalText);
            } else if (isLowerLabel(R->label)) {
                if (streq(R->inst, ".fill")) {
                    unsigned int dataIdx = F->dataStartingLine + R->offset;
                    int V = combinedData[dataIdx];
                    if (V < (int)F->textSize)
                        finalAddr = (int)(F->textStartingLine + V);
                    else {
                        int offInData = V - (int)F->textSize;
                        finalAddr = (int)(totalText + F->dataStartingLine + offInData);
                    }
                } else {
                    unsigned int textIdx = F->textStartingLine + R->offset;
                    int word = combinedText[textIdx];
                    int16_t imm = (int16_t)(word & 0xFFFF);
                    int V = (int)imm;
                    if (V < (int)F->textSize)
                        finalAddr = (int)(F->textStartingLine + V);
                    else {
                        int offInData = V - (int)F->textSize;
                        finalAddr = (int)(totalText + F->dataStartingLine + offInData);
                    }
                }
            } else {
                exit(1);
            }

            if (streq(R->inst, ".fill")) {
                unsigned int dataIdx = F->dataStartingLine + R->offset;
                combinedData[dataIdx] = finalAddr;
            } else if (streq(R->inst, "lw") || streq(R->inst, "sw")) {
                unsigned int textIdx = F->textStartingLine + R->offset;
                int word = combinedText[textIdx];
                word = (word & ~0xFFFF) | (finalAddr & 0xFFFF);
                combinedText[textIdx] = word;
            } else {
                exit(1);
            }
        }
    }

    /* ---- 输出最终机器码 ---- */
    for (unsigned int i = 0; i < totalText; ++i)
        printHexToFile(outFilePtr, combinedText[i]);
    for (unsigned int i = 0; i < totalData; ++i)
        printHexToFile(outFilePtr, combinedData[i]);

    fclose(outFilePtr);
    return 0;
}
