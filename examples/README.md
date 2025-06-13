# Examples for linked-assembly

This folder contains minimal example programs that can be assembled with `assembly`.

## Usage

First, build the assembler:

```bash
make
```

Then, to assembly a specific example (e.g. `example.asm`):

```bash
mkdir -p examples/build
build/assembly/assembly examples/example.asm -o examples/build/example.o --arch x86 --format elf -m32
```

Replace `example.asm` with any example file from this directory.