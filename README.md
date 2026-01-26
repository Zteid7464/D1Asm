# D1 ASM

## An assembler for the D1 computer.

### Note

I know that the code is terrible and probaly not secure!

### How to use

```bash
d1asm <input file> <output file>
```

### Labels

Labels can be declared like this

```asm
myLabel:
```

An instruction can also come right after the label

```asm
myLabel: add 0
```

Labels can be used instead of specifing a address to jump to directly like so

```asm
jnc myLabel
```

### Comments

Everything after a `;` will be ignored by the assembler

```asm
ldi 10 ; i am a comment and the assembler does not care about me :(
```

### Instructions

D1 operators manual comming at some point in the future!
