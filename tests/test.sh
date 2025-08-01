#!/bin/sh

mkdir -p build

../bin/assembly $1.asm -o build/$1_asm.o \
    --arch x86      \
    -m32            \
    --endian little \
    --format elf

../bin/linker build/$1_asm.o -o build/$1

chmod +x build/$1