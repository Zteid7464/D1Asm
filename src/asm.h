#ifndef uint8_t
    #include <stdint.h>
#endif

#ifndef FILE
    #include <stdio.h>
#endif

#define MAX_LABEL_NAME_LENGTH 23
#define NOPARAMOPCODE 0x0

typedef struct {
    char name[MAX_LABEL_NAME_LENGTH+1];   // the name of the label
    uint8_t address;                    // the address in the program that the label corrosponds to
    unsigned line;                      // the line in the assembly file that the lable is defind on
} Label;

typedef struct {
    uint8_t opcode;
    uint8_t parameter;
    char labelName[MAX_LABEL_NAME_LENGTH+1];
} Instruction;

#define INST_COUNT 20

// will check if the line cotains a lable. if it does it will copy the name of the lable to labelName. And return 1. If the line doesen't contain a lable it will return 0.
int isLabel(const char* line, char* labelName);

// will read in all labels in the open source file asmFile into labels. The address atribute will not be set
int readLables(FILE* asmFile, Label labels[64]);

// will check if the line contains a instruction. If it does it will return 1 and will read and assemble the instruction into inst. If it does not i will return 0
int isInstruction(const char* line, Instruction* inst);

// will return the index of the character in s that comes after the first colon. If no colon is found it will return 0;
int skipColon(const char* s);

// string compare with specified end. will compare s1 starting at si1 with s2 starting at si2 util null or term is hit
_Bool strcmpwse(const char* s1, const char* s2, const int si1, const int si2, char term);

// will return 1 if c is a "space", a null terminator or term. Otherwise return 0
_Bool isSpaceTermNull(const char c, const char term);

// will write out the assembled program to file.
int writeProgram(Instruction* program, Label* labels, int programLength, char* programFileName);