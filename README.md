# linked-assembly

A modular assembler.  
Currently only supports x86 32-bit mode and ELF `.o` object files.

## Features

- Supports basic x86 instructions: `nop`, `ret`, `ret imm16`, `int3`, `int1`, `cli`, `sti`, `hlt`, `cmc`, `clc`, `stc`, `std`, `wait/fwait`, `pause`, `rdtsc`, `cpuid`, `mov reg imm32`, `mov reg reg`.
- Generates valid 32-bit ELF object files.
- Simple syntax for instructions.
- Supports the following flags:
  - `--arch <arch>`: target architecture; supported values are `x86`, `arm`, and `riscv`.
  - `--format <format>`: output object file format; supported values are `elf`, `macho`, and `coff`.
  - `-m64`, `-m32`, `-m16`: selects the bit mode.

**Note:** Currently, only `--arch x86`, `--format elf`, and `-m32` are fully supported.

## Building

Build the assembler using `make`:

```bash
make
```

## Usage

Build your assembly file into an object file with:

```bash
build/assembly/assembly input.asm -o output.o --arch x86 --format elf -m32
```