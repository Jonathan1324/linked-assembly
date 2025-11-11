#!/bin/sh

python3 -m ci.ci

cd tests/lfs

mkdir -p build/test_mkfs

OS=$(uname)

dd if=/dev/zero of=build/test_mkfs/fat12.img bs=1024 count=1440
if [ "$OS" = "Darwin" ]; then
    newfs_msdos -F 12 -i 0x12345678 build/test_mkfs/fat12.img
else
    mkfs.fat -F 12 build/test_mkfs/fat12.img -i 0x12345678
fi

dd if=/dev/zero of=build/test_mkfs/fat16.img bs=1M count=16
if [ "$OS" = "Darwin" ]; then
    newfs_msdos -F 16 -i 0x12345678 build/test_mkfs/fat16.img
else
    mkfs.fat -F 16 build/test_mkfs/fat16.img -i 0x12345678
fi

dd if=/dev/zero of=build/test_mkfs/fat32.img bs=1M count=100
if [ "$OS" = "Darwin" ]; then
    newfs_msdos -F 32 -i 0x12345678 build/test_mkfs/fat32.img
else
    mkfs.fat -F 32 build/test_mkfs/fat32.img -i 0x12345678
fi

cd ../..
