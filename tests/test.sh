#!/bin/sh

mkdir -p build

../build/assembler/assembler $1.asm -o build/$1_asm.o \
    --arch x86      \
    -m32            \
    --endian little \
    --format elf

../build/linker/linker build/$1_asm.o -o build/$1

chmod +x build/$1