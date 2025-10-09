#!/bin/sh

python3 -m ci.ci

find "tests/assembler/build/nasm/lasm" -type f -name "*.o" | while read -r file; do
    prefix="tests/assembler/build/nasm/lasm"
    parent_dir_all="$(dirname "$file")"
    parent_dir=$(echo "$parent_dir_all" | sed "s|^$prefix||")
    log_file="logs/assembler/nasm/elf-lasm/$parent_dir/$(basename "$file")-objdump.txt"
    mkdir -p "$(dirname "$log_file")"
    objdump --all-headers "$file" > "$log_file" 2>&1
done

find "tests/assembler/build/nasm/nasm" -type f -name "*.o" | while read -r file; do
    prefix="tests/assembler/build/nasm/nasm"
    parent_dir_all="$(dirname "$file")"
    parent_dir=$(echo "$parent_dir_all" | sed "s|^$prefix||")
    log_file="logs/assembler/nasm/elf-nasm/$parent_dir/$(basename "$file")-objdump.txt"
    mkdir -p "$(dirname "$log_file")"
    objdump --all-headers "$file" > "$log_file" 2>&1
done

#rm -f logs/assembler/cmp.log
#
#if cmp -l tests/assembler/build/test.asm-x86-32bit-bin.bin tests/assembler/build/nasm/test-32bit-bin.bin >> logs/assembler/cmp.log 2>&1; then
#    echo "[INFO] 32bit-bin: equal" >> logs/assembler/cmp.log 2>&1
#else
#    echo "[INFO] 32bit-bin: different" >> logs/assembler/cmp.log 2>&1
#fi
#if cmp -l tests/assembler/build/test.asm-x86-64bit-bin.bin tests/assembler/build/nasm/test-64bit-bin.bin >> logs/assembler/cmp.log 2>&1; then
#    echo "[INFO] 64bit-bin: equal" >> logs/assembler/cmp.log 2>&1
#else
#    echo "[INFO] 64bit-bin: different" >> logs/assembler/cmp.log 2>&1
#fi
#
#if cmp -l tests/assembler/build/test.asm-x86-32bit-elf.o tests/assembler/build/nasm/test-32bit-elf.o >> logs/assembler/cmp.log 2>&1; then
#    echo "[INFO] 32bit-elf: equal" >> logs/assembler/cmp.log 2>&1
#else
#    echo "[INFO] 32bit-elf: different" >> logs/assembler/cmp.log 2>&1
#fi
#if cmp -l tests/assembler/build/test.asm-x86-64bit-elf.o tests/assembler/build/nasm/test-64bit-elf.o >> logs/assembler/cmp.log 2>&1; then
#    echo "[INFO] 64bit-elf: equal" >> logs/assembler/cmp.log 2>&1
#else
#    echo "[INFO] 64bit-elf: different" >> logs/assembler/cmp.log 2>&1
#fi
#
## OS erkennen
#OS=$(uname)
#
#if [ "$OS" = "Linux" ]; then
#    ld -m elf_i386 -o tests/assembler/build/elf/test-32bit tests/assembler/build/test.asm-x86-32bit-elf.o >> logs/assembler/elf/ld-test-32bit.txt 2>&1
#    ld -m elf_x86_64 -o tests/assembler/build/elf/test-64bit tests/assembler/build/test.asm-x86-64bit-elf.o >> logs/assembler/elf/ld-test-64bit.txt 2>&1
#
#    chmod +x tests/assembler/build/elf/test-32bit
#    chmod +x tests/assembler/build/elf/test-64bit
#else
#    echo "[WARNING] Can't link on this OS: $OS" >> logs/assembler/elf/ld.txt 2>&1
#    echo "[WARNING] Can't link on this OS: $OS"
#    exit 0
#fi

#tests/assembler/build/elf/test-32bit
#echo "exit code: $?"
