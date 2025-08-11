#!/bin/sh

sh run.sh -m

chmod +x run.sh

cd tests

../bin/assembler test.asm -o build/test.o \
    --arch x86 --format elf -m32 -d

nasm -f elf32 test.asm -o build/test-nasm.o

objdump --all-headers build/test-nasm.o
objdump --all-headers build/test.o

ld -m elf_i386 -o build/test build/test.o
chmod +x build/test
#build/test
#echo "exit code: $?"

cd ..

sh run.sh test -no-make

cmp tests/build/test.o tests/build/test-nasm.o && echo ".o: equal" || echo ".o: different"