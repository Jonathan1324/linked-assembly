#!/bin/sh

python3 -m ci.ci

cd tests/lfs

mkdir -p build/mkfs
dd if=/dev/zero of=build/mkfs/fat12.img bs=1024 count=1440
mkfs.fat -F 12 build/mkfs/fat12.img -s 1 -S 512 -i 0x12345678

mkdir -p build/test
../../dist/bin/lfs create build/test/fat12.img $@

../../dist/bin/lfs create build/test/fat12_root.img --root roots/test $@

cd ../..