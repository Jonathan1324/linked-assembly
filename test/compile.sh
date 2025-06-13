#!/bin/sh

mkdir -p build

if [ "$1" = "-d" ]; then
  ../build/assembly/assembly main.asm -o build/test.o \
    --arch x86 \
    -m32 \
    --format elf \
    --debug
else
  ../build/assembly/assembly main.asm -o build/test.o \
    --arch x86 \
    -m32 \
    --format elf
fi
