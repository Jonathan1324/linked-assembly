#!/bin/sh

mkdir -p build

../build/assembly/assembly $1 -o $1.o \
    --arch x86      \
    -m32            \
    --format elf    \
    --debug

../build/linker/linker build/$1_asm.o -o build/$1 \
    --debug

chmod +x build/$1