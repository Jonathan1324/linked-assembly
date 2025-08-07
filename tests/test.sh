#!/bin/sh

mkdir -p build

../bin/assembler $1.asm -o build/$1.bin     \
    --arch x86                              \
    -m32                                    \
    --format bin

nasm $1.asm -o build/$1-nasm.bin

cmp build/$1.bin build/$1-nasm.bin && echo "equal" || echo "different"