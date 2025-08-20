#!/bin/sh

python3 -m ci.ci

mkdir -p tests/assembler/build/nasm
mkdir -p tests/assembler/build/elf
mkdir -p logs/assembler/elf
mkdir -p logs/assembler/nasm-verbose

rm -f logs/assembler/nasm-verbose/test.asm-32bit-bin.txt
rm -f logs/assembler/nasm-verbose/test.asm-64bit-bin.txt
rm -f logs/assembler/nasm-verbose/test.asm-32bit-elf.txt
rm -f logs/assembler/nasm-verbose/test.asm-64bit-elf.txt
rm -f logs/assembler/elf/test-nasm-32bit-objdump.txt
rm -f logs/assembler/elf/test-nasm-64bit-objdump.txt
rm -f logs/assembler/elf/test-32bit-objdump.txt
rm -f logs/assembler/elf/test-64bit-objdump.txt
rm -f logs/assembler/elf/ld-test-32bit.txt
rm -f logs/assembler/elf/ld-test-64bit.txt

nasm -f bin tests/assembler/srcs/test.asm -o tests/assembler/build/nasm/test-32bit-bin.bin >> logs/assembler/nasm-verbose/test.asm-32bit-bin.txt 2>&1
nasm -f bin tests/assembler/srcs/test.asm -o tests/assembler/build/nasm/test-64bit-bin.bin >> logs/assembler/nasm-verbose/test.asm-64bit-bin.txt 2>&1
nasm -f elf32 tests/assembler/srcs/test.asm -o tests/assembler/build/nasm/test-32bit-elf.o >> logs/assembler/nasm-verbose/test.asm-32bit-elf.txt 2>&1
nasm -f elf64 tests/assembler/srcs/test.asm -o tests/assembler/build/nasm/test-64bit-elf.o >> logs/assembler/nasm-verbose/test.asm-64bit-elf.txt 2>&1

objdump --all-headers tests/assembler/build/nasm/test-32bit-elf.o >> logs/assembler/elf/test-nasm-32bit-objdump.txt 2>&1
objdump --all-headers tests/assembler/build/nasm/test-64bit-elf.o >> logs/assembler/elf/test-nasm-64bit-objdump.txt 2>&1
objdump --all-headers tests/assembler/build/tests/assembler/srcs/test.asm-x86-32bit-elf.o >> logs/assembler/elf/test-32bit-objdump.txt 2>&1
objdump --all-headers tests/assembler/build/tests/assembler/srcs/test.asm-x86-64bit-elf.o >> logs/assembler/elf/test-64bit-objdump.txt 2>&1

rm -f logs/assembler/cmp.log

if cmp tests/assembler/build/tests/assembler/srcs/test.asm-x86-32bit-bin.bin tests/assembler/build/nasm/test-32bit-bin.bin >> logs/assembler/cmp.log 2>&1; then
    echo "[INFO] 32bit-bin: equal" >> logs/assembler/cmp.log 2>&1
else
    echo "[INFO] 32bit-bin: different" >> logs/assembler/cmp.log 2>&1
fi
if cmp tests/assembler/build/tests/assembler/srcs/test.asm-x86-64bit-bin.bin tests/assembler/build/nasm/test-64bit-bin.bin >> logs/assembler/cmp.log 2>&1; then
    echo "[INFO] 64bit-bin: equal" >> logs/assembler/cmp.log 2>&1
else
    echo "[INFO] 64bit-bin: different" >> logs/assembler/cmp.log 2>&1
fi

if cmp tests/assembler/build/tests/assembler/srcs/test.asm-x86-32bit-elf.o tests/assembler/build/nasm/test-32bit-elf.o >> logs/assembler/cmp.log 2>&1; then
    echo "[INFO] 32bit-elf: equal" >> logs/assembler/cmp.log 2>&1
else
    echo "[INFO] 32bit-elf: different" >> logs/assembler/cmp.log 2>&1
fi
if cmp tests/assembler/build/tests/assembler/srcs/test.asm-x86-64bit-elf.o tests/assembler/build/nasm/test-64bit-elf.o >> logs/assembler/cmp.log 2>&1; then
    echo "[INFO] 64bit-elf: equal" >> logs/assembler/cmp.log 2>&1
else
    echo "[INFO] 64bit-elf: different" >> logs/assembler/cmp.log 2>&1
fi

# OS erkennen
OS=$(uname)

if [ "$OS" = "Linux" ]; then
    ld -m elf_i386 -o build/elf/test-32bit tests/assembler/build/tests/assembler/srcs/test.asm-x86-32bit-elf.o >> logs/assembler/elf/ld-test-32bit.txt 2>&1
    ld -m elf_x86_64 -o build/elf/test-64bit tests/assembler/build/tests/assembler/srcs/test.asm-x86-64bit-elf.o >> logs/assembler/elf/ld-test-64bit.txt 2>&1

    chmod +x build/test
else
    echo "[WARNING] Can't link on this OS: $OS" >> logs/assembler/elf/ld.txt 2>&1
    echo "[WARNING] Can't link on this OS: $OS"
    exit 0
fi

#build/test
#echo "exit code: $?"
