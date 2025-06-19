#!/bin/sh

make DEBUG=1

chmod +x build/assembly/assembly

cd tests

mkdir -p build
../build/assembly/assembly test.asm -o build/test.o \
    --arch x86 --format raw -m32

#objdump --all-headers build/test.o

nasm test.asm -o build/nasm.o

cd ..