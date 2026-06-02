# RISC-V Instruction Simulator

## Overview

This project is a basic RISC-V instruction simulator written in C++. It reads 32-bit binary instructions from a text file and simulates the main stages of instruction execution, including fetch, decode, execute, memory access, and write-back.

The simulator keeps track of the program counter, register file, data memory, control signals, ALU results, memory updates, register changes, and total clock cycles.

 Features

* Reads RISC-V binary instructions from an input text file
* Simulates instruction execution step by step
* Tracks and updates the program counter
* Uses a 32-register register file
* Supports basic data memory operations
* Prints register, memory, and PC modifications
* Counts total clock cycles during program execution

# Supported Instruction Types

This simulator supports several RISC-V instruction formats:
* R-type instructions
* I-type instructions
* S-type instructions
* SB-type branch instructions
* UJ-type jump instructions

# Supported Instructions

Examples of supported instructions include:

* `add`
* `sub`
* `and`
* `or`
* `xor`
* `sll`
* `srl`
* `sra`
* `slt`
* `sltu`
* `addi`
* `andi`
* `ori`
* `lw`
* `sw`
* `beq`
* `bne`
* `blt`
* `bge`
* `jal`
* `jalr`

# Concepts Used

* RISC-V instruction formats
* Opcode, funct3, and funct7 decoding
* Register file operations
* Program counter updates
* ALU operations
* Branch and jump logic
* Memory read and write operations
* Control signal generation
* Clock cycle tracking

# How to Run

Compile the program using a C++ compiler:

```bash
g++ tdayafleh.cpp -o riscv_simulator
```

Run the executable:

```bash
./riscv_simulator
```

When prompted, enter the name of the input file:

```text
sample_part1.txt
```

or

```text
sample_part2.txt
```

# Input Format

The input file should contain one 32-bit binary instruction per line.

Example:

```text
00000000001000001000000110110011
```

# Output

The program prints the execution process, including:

* Total clock cycle number
* Register modifications
* Memory modifications
* Program counter updates
* Final total execution time



