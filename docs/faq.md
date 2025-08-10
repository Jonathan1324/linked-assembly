# FAQ — linked-assembly

Frequently Asked Questions about the `linked-assembly` project.

---

## ❓ Why only x86 ELF for now?

The project started with a narrow focus to ensure correctness and simplicity.  
x86 is relatively well-documented, widely supported, and easier to implement than  newer ISAs like ARM and RISC-V.

Support for other architectures and formats is planned.

---

## ❓ Can I write real programs with this assembler?

Yes — within limits. You can write minimal x86 assembly programs that don’t rely on complex features.

Currently, you can:

- Write short functions or snippets.
- Generate object files that can be linked with `gcc`.

---

## ❓ How do I run the output?

You assemble your `.asm` file into an `.o` file using `assembler`, then link it with a linker like `gcc`:

```bash
bin/assembler file.asm -o file.o --arch x86 --format elf -m32
gcc file.o -o file
./file
```

Use `echo $?` to see the return value (if applicable).

---

## ❓ Why is instruction support so limited?

Instruction support doesn't come out of thin air.

More will be added as the encoding logic expands. You can help contribute! (See [CONTRIBUTING.md](../CONTRIBUTING.md))

---

## ❓ How do I add support for a new instruction?

You’ll need to:

1. Update the parser to recognize the instruction.
2. Add encoding logic in the instruction encoder.
3. Optionally, add a test/example.

See [CONTRIBUTING.md](../CONTRIBUTING.md) for details.

---

## ❓ Is there a test suite?

Not yet — but it's planned. For now, you can test behavior manually by assembling and linking small test cases.
Adding automated tests is high-priority.

---

## ❓ How can I help?

Contributions are welcome!
See [CONTRIBUTING.md](../CONTRIBUTING.md) for how to get started, and look at open issues or propose improvements.

---

Have a question that’s not here?
Open an issue or start a discussion!
