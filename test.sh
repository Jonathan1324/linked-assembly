#!/bin/sh

make DEBUG=1

chmod +x bin/assembly
chmod +x bin/linker

cd tests

mkdir -p build
../bin/assembly test.asm -o build/test.bin \
    --arch x86 --format raw -m32

../bin/assembly test.asm -o build/test.o \
    --arch x86 --format elf -m32

ld -m elf_i386 -o build/test build/test.o

chmod +x build/test

#objdump --all-headers build/test.o

nasm test.asm -o build/nasm.bin

build/test

echo $?

cmp build/test.bin build/nasm.bin && echo "equal" || echo "different"

cd ..