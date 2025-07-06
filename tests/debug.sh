#!/bin/sh

mkdir -p build

../build/assembler/assembler $1 -o $1.o \
    --arch x86      \
    -m32            \
    --format macho  \
    --endian little \
    --debug

../build/linker/linker build/$1_asm.o -o build/$1 \
    --debug

chmod +x build/$1