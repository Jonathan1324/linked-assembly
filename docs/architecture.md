# Architecture of linked-assembly

## Overview

linked-assembly is a modular assembler designed to translate human-readable assembly code into machine-readable object files.  
Currently, it supports the **x86 architecture in 32-bit mode** and outputs **ELF `.o` object files**.

## Modular Design

The assembler consists of several key components:

- **Parser:**  
  Reads and interprets the assembly source code, recognizing instructions, registers, immediates, and directives.

- **Instruction Encoder:**  
  Translates parsed instructions into their corresponding machine code bytes according to the x86 32-bit instruction set.

- **Object File Writer:**  
  Packages the generated machine code into a valid ELF object file format (`.o`), suitable for linking.

## Supported Architectures and Formats

- Currently, only **x86 32-bit mode** is fully supported.
- Output is limited to **ELF object files**.
- Command-line flags allow specifying other architectures (`arm`, `riscv`) and formats (`macho`, `coff`), but these are placeholders for future development.

## Design Decisions

- **Why x86 32-bit ELF?**  
  x86 remains a widely used architecture and 32-bit mode simplifies instruction encoding. ELF is the standard object format on Linux systems.

- **Modularity:**  
  The assembler is designed to be extensible. Adding support for new architectures or output formats involves implementing new parsers, encoders, or file writers without rewriting the entire system.

- **Simplicity:**  
  The current instruction set is minimal, focusing on correctness and clarity before expanding.

## Future Architecture Goals

- Full support for **ARM** and **RISC-V** architectures.
- Support for additional output formats like **Mach-O** (macOS) and **COFF** (Windows).
- Enhanced instruction parsing with macros, addresses, and expressions.

---

*This document will be updated as the project evolves.*
