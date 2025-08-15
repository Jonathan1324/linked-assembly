#!/bin/sh

python3 -m ci.ci

mkdir -p tests/build/nasm
mkdir -p tests/build/elf
mkdir -p logs/elf
mkdir -p logs/nasm-verbose

rm -f logs/nasm-verbose/test.asm-32bit-bin.txt
rm -f logs/nasm-verbose/test.asm-64bit-bin.txt
rm -f logs/nasm-verbose/test.asm-32bit-elf.txt
rm -f logs/nasm-verbose/test.asm-64bit-elf.txt
rm -f logs/elf/test-nasm-32bit-objdump.txt
rm -f logs/elf/test-nasm-64bit-objdump.txt
rm -f logs/elf/test-32bit-objdump.txt
rm -f logs/elf/test-64bit-objdump.txt
rm -f logs/elf/ld-test-32bit.txt
rm -f logs/elf/ld-test-64bit.txt

nasm -f bin tests/srcs/test.asm -o tests/build/nasm/test-32bit-bin.bin >> logs/nasm-verbose/test.asm-32bit-bin.txt 2>&1
nasm -f bin tests/srcs/test.asm -o tests/build/nasm/test-64bit-bin.bin >> logs/nasm-verbose/test.asm-64bit-bin.txt 2>&1
nasm -f elf32 tests/srcs/test.asm -o tests/build/nasm/test-32bit-elf.o >> logs/nasm-verbose/test.asm-32bit-elf.txt 2>&1
nasm -f elf64 tests/srcs/test.asm -o tests/build/nasm/test-64bit-elf.o >> logs/nasm-verbose/test.asm-64bit-elf.txt 2>&1

objdump --all-headers tests/build/nasm/test-32bit-elf.o >> logs/elf/test-nasm-32bit-objdump.txt 2>&1
objdump --all-headers tests/build/nasm/test-64bit-elf.o >> logs/elf/test-nasm-64bit-objdump.txt 2>&1
objdump --all-headers tests/build/srcs/test.asm-x86-32bit-elf.o >> logs/elf/test-32bit-objdump.txt 2>&1
objdump --all-headers tests/build/srcs/test.asm-x86-64bit-elf.o >> logs/elf/test-64bit-objdump.txt 2>&1

rm -f logs/cmp.log

if cmp tests/build/srcs/test.asm-x86-32bit-bin.bin tests/build/nasm/test-32bit-bin.bin >> logs/cmp.log 2>&1; then
    echo "[INFO] 32bit-bin: equal" >> logs/cmp.log 2>&1
else
    echo "[INFO] 32bit-bin: different" >> logs/cmp.log 2>&1
fi
if cmp tests/build/srcs/test.asm-x86-64bit-bin.bin tests/build/nasm/test-64bit-bin.bin >> logs/cmp.log 2>&1; then
    echo "[INFO] 64bit-bin: equal" >> logs/cmp.log 2>&1
else
    echo "[INFO] 64bit-bin: different" >> logs/cmp.log 2>&1
fi

if cmp tests/build/srcs/test.asm-x86-32bit-elf.o tests/build/nasm/test-32bit-elf.o >> logs/cmp.log 2>&1; then
    echo "[INFO] 32bit-elf: equal" >> logs/cmp.log 2>&1
else
    echo "[INFO] 32bit-elf: different" >> logs/cmp.log 2>&1
fi
if cmp tests/build/srcs/test.asm-x86-64bit-elf.o tests/build/nasm/test-64bit-elf.o >> logs/cmp.log 2>&1; then
    echo "[INFO] 64bit-elf: equal" >> logs/cmp.log 2>&1
else
    echo "[INFO] 64bit-elf: different" >> logs/cmp.log 2>&1
fi

# OS erkennen
OS=$(uname)

if [ "$OS" = "Linux" ]; then
    ld -m elf_i386 -o build/elf/test-32bit tests/build/srcs/test.asm-x86-32bit-elf.o >> logs/elf/ld-test-32bit.txt 2>&1
    ld -m elf_x86_64 -o build/elf/test-64bit tests/build/srcs/test.asm-x86-64bit-elf.o >> logs/elf/ld-test-64bit.txt 2>&1

    chmod +x build/test
else
    echo "[ERROR] Can't link on this OS: $OS" >> logs/elf/ld.txt 2>&1
    echo "[ERROR] Can't link on this OS: $OS"
    exit 0
fi

#build/test
#echo "exit code: $?"
