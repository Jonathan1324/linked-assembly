#!/bin/sh

python3 -m ci.ci

cd tests/lfs

mkdir -p build/test_mkfs
dd if=/dev/zero of=build/test_mkfs/fat12.img bs=1024 count=1440
mkfs.fat -F 12 build/test_mkfs/fat12.img -s 1 -S 512 -i 0x12345678
dd if=/dev/zero of=build/test_mkfs/fat16.img bs=1M count=16
mkfs.fat -F 16 build/test_mkfs/fat16.img -i 0x12345678
dd if=/dev/zero of=build/test_mkfs/fat32.img bs=1M count=100
mkfs.fat -F 32 build/test_mkfs/fat32.img -i 0x12345678

cd ../..