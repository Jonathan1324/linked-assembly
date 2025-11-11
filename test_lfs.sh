#!/bin/sh

python3 -m ci.ci

cd tests/lfs

mkdir -p build/test_mkfs

OS=$(uname)

dd if=/dev/zero of=build/test_mkfs/fat12.img bs=1024 count=1440
if [ "$OS" = "Darwin" ]; then
    MDDEV=$(sudo mdconfig -a -t vnode -f build/test_mkfs/fat12.img)
    sudo newfs_msdos -F 12 "/dev/$MDDEV" -i 0x12345678
    sudo mdconfig -d -u "$MDDEV"
else
    mkfs.fat -F 12 build/test_mkfs/fat12.img -i 0x12345678
fi

dd if=/dev/zero of=build/test_mkfs/fat16.img bs=1M count=16
if [ "$OS" = "Darwin" ]; then
    MDDEV=$(sudo mdconfig -a -t vnode -f build/test_mkfs/fat16.img)
    sudo newfs_msdos -F 16 "/dev/$MDDEV" -i 0x12345678
    sudo mdconfig -d -u "$MDDEV"
else
    mkfs.fat -F 16 build/test_mkfs/fat16.img -i 0x12345678
fi

dd if=/dev/zero of=build/test_mkfs/fat32.img bs=1M count=100
if [ "$OS" = "Darwin" ]; then
    MDDEV=$(sudo mdconfig -a -t vnode -f build/test_mkfs/fat32.img)
    sudo newfs_msdos -F 32 "/dev/$MDDEV" -i 0x12345678
    sudo mdconfig -d -u "$MDDEV"
else
    mkfs.fat -F 32 build/test_mkfs/fat32.img -i 0x12345678
fi

cd ../..
