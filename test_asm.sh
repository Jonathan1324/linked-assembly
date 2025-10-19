#!/bin/sh

python3 -m ci.ci

find "tests/assembler/build/nasm/lasm" -type f -name "*.o" | while read -r file; do
    prefix="tests/assembler/build/nasm/lasm"
    parent_dir_all="$(dirname "$file")"
    parent_dir=$(echo "$parent_dir_all" | sed "s|^$prefix||")
    log_file="logs/assembler/nasm/objdump-lasm/$parent_dir/$(basename "$file")-objdump.txt"
    mkdir -p "$(dirname "$log_file")"
    objdump --all-headers "$file" > "$log_file" 2>&1
done

find "tests/assembler/build/nasm/nasm" -type f -name "*.o" | while read -r file; do
    prefix="tests/assembler/build/nasm/nasm"
    parent_dir_all="$(dirname "$file")"
    parent_dir=$(echo "$parent_dir_all" | sed "s|^$prefix||")
    log_file="logs/assembler/nasm/objdump-nasm/$parent_dir/$(basename "$file")-objdump.txt"
    mkdir -p "$(dirname "$log_file")"
    objdump --all-headers "$file" > "$log_file" 2>&1
done
