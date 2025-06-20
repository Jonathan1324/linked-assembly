# Usage Guide for linked-assembly

## Building the Assembler

To build the assembler binary, simply run:

```bash
make
```

This will compile the source code and produce the binary at:

```bash
build/assembly/assembly
```

---

## Basic Usage

To assemble an assembly source file (`input.asm`) into an object file (`output.o`), use the following command:

```bash
build/assembly/assembly input.asm -o output.o --arch x86 --format elf -m32
```

### Command-Line Flags

* `--arch <arch>`
  Specify the target architecture. Supported values:

  * `x86` (currently fully supported)
  * `arm` (planned)
  * `riscv` (planned)

* `--format <format>`
  Specify the output file format. Supported values:

  * `elf` (currently fully supported)
  * `macho` (planned)
  * `coff` (planned)

* `--endian <endianness>`
  Specify the target endianness. Supported values:

  * `little` (currently fully supported)
  * `big` (currently not fully supported)

* `-m64`, `-m32`, `-m16`
  Select the bit mode (64-bit, 32-bit, or 16-bit). Currently only `-m32` is supported.

* `-v` / `--version`
  Show version information.

* `-h` / `--help`
  Show help message.

---

## Example

Given an assembly source file `examples/example.asm`, assemble it like this:

```bash
build/assembly/assembly examples/example.asm -o examples/build/example.o --arch x86 --format elf -m32
```

---

## Notes & Limitations

* Currently, only the x86 architecture in 32-bit mode with ELF or binary output is implemented.
* The assembler supports a minimal subset of x86 instructions. See [instructions.md](instructions.md) for the full list.
* Error handling and diagnostics are basic and will be improved in future releases.

---

## Troubleshooting

* If the assembler fails to build, ensure you have a compatible `make` and compiler installed.
* For unsupported instructions or syntax errors, double-check the supported instructions and syntax.
* Feel free to open an issue if you encounter unexpected behavior.

---

*This document will be updated as the project matures.*
