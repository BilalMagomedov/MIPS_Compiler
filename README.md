## MIPS Assembly Compiler

### Overview

This C++ program acts as a MIPS assembly compiler, converting MIPS assembly code from a file (in ".asm" format) into hexadecimal instruction codes and generating an ".obj" file with addresses for each instruction. The compiler supports R, J, and I-type instructions and includes additional features such as handling extra spaces and ignoring line comments within instructions.

### Input File Format

The input file should follow the MIPS assembly language syntax. The program supports R, J, and I-type instructions. Additionally, the compiler accommodates extra spaces and ignores line comments within instructions.

### Program Execution

1. The program reads instructions from the input file ("mips.asm").
2. It processes R, J, and I-type instructions, handling extra spaces and ignoring line comments.
3. The program outputs the instruction codes in hex format along with addresses in the ".obj" file.

### Input File Example (mips.asm)

```asm
.text
again: add $11, $12, $23
show:  addi    $8, $7, -1234
        andi    $3, $7,     127
        beq     $8, $10, show  
        bne     $4, $6, xl
xl:     sll     $17, $18, 4
        j       again
```

### Output File Example (output.obj)

```
ADDRESS         CODE
0x00400000	0x01975820
0x00400004	0x20e8fb2e
0x00400008	0x30e3007f
0x0040000c	0x110afffd
0x00400010	0x14860000
0x00400014	0x00128900
0x00400018	0x08100000
```

### How to Run

1. Compile the C++ program.
2. Place the input file ("mips.asm") in the same directory as the executable.
3. Run the compiled program.
4. Check the generated ".obj" file for the compiled instructions and addresses.

### Additional Notes

- Ensure the input file follows the MIPS assembly language syntax.
- The program supports R, J, and I-type instructions.
- Extra spaces and line comments within instructions are handled.

### Screenshots

![image](https://github.com/BilalMagomedov/MIPS_Compiler/assets/53838499/e3bb98a2-33a0-4fed-a9ef-6bbe5facbad2)

![image](https://github.com/BilalMagomedov/MIPS_Compiler/assets/53838499/ef1eedad-2ebc-4267-a4e5-77d3529c0014)

### Author

[Bilal Magomedov]

### Project Date

[01.01.2024]
