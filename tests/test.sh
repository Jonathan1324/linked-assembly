#!/bin/sh

mkdir -p build

../bin/asmp $1.asm -o build/$1.i.asm

../bin/assembler build/$1.i.asm -o build/$1_asm.o \
    --arch x86      \
    -m32            \
    --endian little \
    --format elf

../bin/linker build/$1_asm.o -o build/$1

chmod +x build/$1