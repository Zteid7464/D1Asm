#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "asm.h"

int main (int argc, char* argv[]) {
    Label labels[64] = {0};
    Instruction program[64] = {0};

    char line[512] = {0};

    FILE* asmFile = NULL;

    if (argc < 3) {
        printf("Use the correct format you dumbass! Foramt: inputFile outputFile\n");
        return -1;
    }

    if ((asmFile = fopen(argv[1], "r")) == NULL) {
        printf("Could not read file %s!\n", argv[1]);
        return -1;
    }

    readLables(asmFile, labels);

    rewind(asmFile);

    unsigned currentLine = 0;
    unsigned currentAddress = 0;
    while (fgets(line, 512, asmFile) != NULL) {
        printf("LINE %d: ", currentLine);
        // give the labels addresses
        for (int i = 0; i < 64; i++) {
            if (labels[i].line==currentLine)
                labels[i].address = currentAddress;
        }

        if (isInstruction(line, &program[currentAddress])) {
            currentAddress++;   // only update the address if we have an actual instruction
        }

        printf ("\n");
        currentLine++;
    }

    fclose(asmFile);

    if (writeProgram(program, labels, 64, argv[2]) != 0) printf("ERROR!\n");

    return 0;
}