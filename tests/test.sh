#!/bin/sh

mkdir -p build

../bin/assembler $1.asm -o build/$1_asm.o \
    --arch x86      \
    -m32            \
    --format elf

../bin/linker build/$1_asm.o -o build/$1

chmod +x build/$1
