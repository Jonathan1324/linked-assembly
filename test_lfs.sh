#!/bin/sh

python3 -m ci.ci

cd tests/lfs

mkdir -p build/mkfs
dd if=/dev/zero of=build/mkfs/fat12.img bs=1024 count=1440
mkfs.fat -F 12 build/mkfs/fat12.img -s 1 -S 512 -i 0x12345678
dd if=/dev/zero of=build/mkfs/fat16.img bs=1M count=16
mkfs.fat -F 16 build/mkfs/fat16.img -i 0x12345678
dd if=/dev/zero of=build/mkfs/fat32.img bs=1M count=100
mkfs.fat -F 32 build/mkfs/fat32.img -i 0x12345678

mkdir -p build/test

../../dist/bin/lfs create build/test/fat12.img --type fat12 --root roots/test --size 1440K

../../dist/bin/lfs create build/test/fat16.img --type fat16 --root roots/test --size 16M

../../dist/bin/lfs create build/test/fat32.img --type fat32 --root roots/test --size 100M

cd ../..