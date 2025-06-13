# linked-assembly

A modular assembler (and linker planned).  
Currently only supports **x86**, **32-bit mode**, and **ELF `.o` object files**.

## Documentation

- [Usage Guide](docs/usage.md)
- [Architecture](docs/architecture.md)
- [Tutorial](docs/tutorial.md)
- [FAQ](docs/faq.md)
- [Instructions](docs/instructions.md)

## Features

- Supports a basic set of x86 instructions ([see full list](docs/instructions.md))
- Generates valid ELF object files.
- Simple, readable assembly syntax
- Command-line flags for extensibility:
  - `--arch <arch>` — target architecture (`x86`, `arm`, `riscv`)
  - `--format <format>` — output file format (`elf`, `macho`, `coff`)
  - `-m64`, `-m32`, `-m16` — selects the bit mode

**Note:** Currently, only `--arch x86`, `--format elf`, and `-m32` are fully supported.

## Building

Build the assembler using `make`:

```bash
make
```
This will produce the binary in `build/assembly/assembly`.

## Usage

Build your assembly file into an object file with:

```bash
build/assembly/assembly input.asm -o output.o --arch x86 --format elf -m32
```

## Contributing

Contributions are very welcome!  
Check out [CONTRIBUTING.md](CONTRIBUTING.md) for how to get started.

## Examples

See examples in the [examples/README.md](examples/README.md).

## License

This project is licensed under the [BSD 3-Clause License](LICENSE).