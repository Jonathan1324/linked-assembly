#!/bin/sh

make all
chmod +x build/assembly/assembly
chmod +x test/compile.sh

cd test
sh compile.sh $@
cd ..

if [ "$1" = "-d" ]; then
  objdump --all-headers test/build/test.o
  /opt/homebrew/opt/binutils/bin/readelf -a test/build/test.o
fi
