#!/bin/sh

mkdir -p build

../build/assembly/assembly $1 -o $1.o \
    --arch x86      \
    -m32            \
    --format elf    \
    --debug