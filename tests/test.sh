#!/bin/sh

mkdir -p build

../build/assembly/assembly $1.asm -o build/$1_asm.o \
    --arch x86      \
    -m32            \
    --format elf

../build/linker/linker build/$1_asm.o -o build/$1

chmod +x build/$1