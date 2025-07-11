#!/bin/sh

make DEBUG=1

chmod +x build/assembly/assembly

cd tests

mkdir -p build
../build/assembly/assembly test.asm -o build/test.bin \
    --arch x86 --format raw -m32

../build/assembly/assembly test.asm -o build/test.o \
    --arch x86 --format elf -m32

ld -m elf_i386 -o build/test build/test.o

chmod +x build/test

#objdump --all-headers build/test.o

nasm test.asm -o build/nasm.bin

build/test

echo $?

cmp build/test.bin build/nasm.bin && echo "equal" || echo "different"

cd ..