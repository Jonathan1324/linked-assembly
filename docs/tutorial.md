# Tutorial: Your First Program with linked-assembly

This tutorial will guide you through writing, assembling, linking, and running a simple program using **linked-assembly**.

---

## Step 1: Write Your First Assembly Program

Create a file named `ret42.asm` with the following content:

```asm
; Simple program: return 42

mov eax, 42
ret
```

This sets the return value to `42` and then returns from the function.

---

## Step 2: Build the Assembler

If you haven’t already, build the assembler with:

```bash
make
```

This will generate the assembler binary at:

```bash
build/assembler/assembler
```

---

## Step 3: Assemble the Program

Use the assembler to compile your `.asm` file into an object file:

```bash
build/assembler/assembler ret42.asm -o ret42.o --arch x86 --format elf --endian little -m32
```

---

## Step 4: Link and Run

Link the object file using `gcc` (or `ld`) to produce an executable:

```bash
gcc ret42.o -o ret42
./ret42
```

You won't see any output, but you can check the program’s return code:

```bash
echo $?
```

It should print:

```bash
42
```

---

## Step 5: Troubleshooting

* Make sure you ran `make` and the `build/assembler/assembler` binary exists.
* Ensure your assembly code only uses supported instructions (see [instructions.md](instructions.md)).
* If you get errors during linking, verify you’re producing valid ELF 32-bit `.o` files and using `-m32` if required.

---

## What's Next?

* Try using more instructions.
* Write a small function with multiple operations.
* Look at the examples in the [`examples/`](../examples) folder.
* Contribute your own example or test case!
