CC := gcc

SRC := ./src/main.c ./src/asm.c
HEADERS := ./src/asm.h

C_FLAGS := -Wall -Wextra -std=c11

OUT := d1asm

$(OUT): $(SRC) $(HEADERS)
	$(CC) $(C_FLAGS) $(SRC) -o $(OUT)