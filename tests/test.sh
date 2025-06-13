#!/bin/sh

mkdir -p build

../build/assembly/assembly $1.asm -o build/$1.asm.o \
    --arch x86      \
    -m32            \
    --format elf