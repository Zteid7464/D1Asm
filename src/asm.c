#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "asm.h"

// every instruction memonic is 4 chars long
char InstList[] = "ADD  SUB  XOR  OR   NOT  RSH  LSH  RRO  LRO  LDI  MDS  MDR  MRS  JNC  CALL RET  IN   OUT  BELL HALT";
uint8_t InstOpCode[] = {0x10, 0x20, 0x30, 0x40, 0x03, 0x04, 0x05, 0x06, 0x07, 0x50, 0x02, 0x60, 0x70, 0x80, 0xC0, 0x01, 0x08, 0x09, 0x0a, 0x00};

int isLabel(const char* line, char* labelName) {
    int pos = 0;
    char c = line[pos];
    char tempStr[MAX_LABEL_NAME_LENGTH+1] = {0};

    int labelLength = 0;
    _Bool characters = 0;
    _Bool potientialSyntaxError = 0;
    _Bool potentialLengthError = 0;

    while (1) {
        if (!isblank(c))    // this detects if there was allready a non blank character
            characters = 1;

        switch (c) {
            case ':':   // if we encounter a : we have a label and we are done
                strncpy(labelName, tempStr, MAX_LABEL_NAME_LENGTH);

                if (potientialSyntaxError)  // if we have a white space inbetween the label that is not allowed
                    printf("Syntax ERROR!\n");
                if (potentialLengthError)
                    printf("Error Label name is to long!\n");
                return 1;

            case '\n':  // if we encounter a new line a comment or a null, before a colon we dont have a label
            case ';':
            case 0:
                return 0;

            case '\t':  // skip tabs and spaces if we encounter a blank after we already got somthing else that is a potential syntax error
            case ' ':
                if (characters != 0)
                    potientialSyntaxError = 1;
                break;
            default:    // if none of the above are true just increase the label lentgh
                labelLength++;
                if (labelLength<MAX_LABEL_NAME_LENGTH)
                    tempStr[labelLength-1] = c;
                else
                    potentialLengthError = 1;
                break;
        }

        pos++;
        c = line[pos];
    }
}

int readLables(FILE* asmFile, Label labels[64]) {
    char line[512] = {0};
    unsigned currentLine = 0;
    int labelCount = 0;

    while (fgets(line, 512, asmFile) != NULL) {     // loop over every line in the file
        if (isLabel(line, labels[labelCount].name)) {   // if the line contains a label store it
            labels[labelCount].line = currentLine;                 // also store the line
            labelCount++;
        }

        currentLine++;
    }

    return 0;
}

int isInstruction(const char *line, Instruction *inst) {
    char cpyLine[512] = {0};    // make a copy of line so we can convert it to uppercase
    strncpy(cpyLine, line, 512);
    
    for (int i = 0; i < 512; i++) {
        if (isalpha(cpyLine[i]))
            cpyLine[i] = toupper(cpyLine[i]);
    }

    int currentChar = skipColon(cpyLine);

    // skip leading newlines
    while (isblank(cpyLine[currentChar]))
        currentChar++;

    // check if the word is equal to any of our instructions
    for (int i = 0; i < INST_COUNT; i++) {
        if (strcmpwse(cpyLine, InstList, currentChar, i*5, ' ')) { // i*4 because there is an instruction every 5 chars in InstList
            inst->opcode = InstOpCode[i];
            printf("%hhx", inst->opcode);

            // use the original version of line again so the case of labels is preserved
            // get the parameters
            // first wait until there is a new white space 
            while (!isblank(line[currentChar]))
                currentChar++;

            // then skip white space again
            while (isblank(line[currentChar]))
                currentChar++;

            // check if it is a label or a numeric constant
            if (isalpha(line[currentChar])) {    // Label
                int i = 0;
                while (!isblank(line[currentChar]) && line[currentChar] != '\n' && line[currentChar] != 0 && line[currentChar] != ';' && i < MAX_LABEL_NAME_LENGTH) {    // copy the label over. stop when we hit whitespaces or a coment or a newline
                    inst->labelName[i] = line[currentChar];
                    currentChar++;
                    i++;
                }
            } else if (isdigit(line[currentChar])) { // numeric constant
                char number[] = {line[currentChar], line[currentChar+1], 0};    // move the number into an string so we can use sscanf
                sscanf(number, "%hhd", &inst->parameter);
                if (inst->parameter > 15 && inst->opcode < 0x80) {
                    printf("ERROR parameter %d to big!\n", inst->parameter);
                } else if (inst->parameter > 63) {
                    printf("ERROR address %d to big!\n", inst->parameter);
                } else if (inst->opcode >> 4 == NOPARAMOPCODE) {
                    printf("ERROR this instruction does not have parameters");
                }
            }

            return 1; 
        }
    }

    return 0;
}


int skipColon(const char* s) {
    int pos = 0;
    char c = s[pos];

    while (1) {
        switch (c) {
            case ':':   // in case we find a colon return the position after it
                return pos+1;
            case '\n':
            case 0:
                return 0;

        }

        pos++;
        c = s[pos];
    }
}

_Bool strcmpwse(const char* s1, const char* s2, const int si1, const int si2, char term) {
    int i1 = si1;
    int i2 = si2;

    while (s1[i1]==s2[i2] || (s1[i1] == '\n' || s2[i2] == '\n') || (s1[i1] == 0 || s2[i2] == 0)) {  // do this because it can be 'equal even if one of them is whitespace or null'
        // if both s1 and s2 are equal to the terminator or null they are equal. You love this line right?
        if ((s1[i1] == 0 && s2[i2] == 0) || (s1[i1] == term && s2[i2] == term) || (s1[i1] == '\n' && s2[i2] == term) || (s1[i1] == term && s2[i2] == '\n') || (s1[i1] == 0 && s2[i2] == term) || (s1[i1] == term && s2[i2] == 0)) {
            return 1;
        }

        i1++;
        i2++;
    }


    return 0;
}

int writeProgram(Instruction* program, Label* labels, int programLength, char* programFileName) {
    FILE* programFile = 0;

    if ((programFile = fopen(programFileName, "w")) == NULL) {
        printf("ERROR while opening file!\n");
        return -1;
    }
    
    for (int i = 0; i < programLength; i++) {
        Instruction currentInst = program[i];

        if (currentInst.opcode >> 4 == NOPARAMOPCODE) {  // no parameter instructions
            fwrite(&currentInst.opcode, 1, 1, programFile);
        } else if (currentInst.opcode < 0x80) { // numerric constant instructions
            uint8_t fullInst = currentInst.opcode | currentInst.parameter;  // merge them togeter
            fwrite(&fullInst, 1, 1, programFile);
        } else if (currentInst.opcode >= 0x80) { // address instructions
            uint8_t fullInst = currentInst.opcode;

            // if there is no label use the parameter
            if (currentInst.labelName[0] == 0) {
                fullInst |= currentInst.parameter;
            } else {    // search for the label
                for (int labelI = 0; labelI < 64; labelI++) {
                    if (strncmp(labels[labelI].name, currentInst.labelName, MAX_LABEL_NAME_LENGTH) == 0) {  // we found our label
                        fullInst |= labels[labelI].address;
                        break;
                    }
                }
            }

            fwrite(&fullInst, 1, 1, programFile);
        }
    }

    fclose(programFile);

    return 0;
}