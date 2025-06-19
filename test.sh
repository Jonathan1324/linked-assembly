#!/bin/sh

make DEBUG=1

chmod +x build/assembly/assembly

cd tests

mkdir -p build
../build/assembly/assembly test.asm -o build/test.bin \
    --arch x86 --format raw -m32

#objdump --all-headers build/test.o

nasm test.asm -o build/nasm.bin

cmp build/test.bin build/nasm.bin && echo "equal" || echo "different"

cd ..