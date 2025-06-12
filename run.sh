make all
chmod +x build/assembly/assembly
chmod +x test/compile.sh
cd test
source compile.sh
cd ..
objdump --all-headers test/build/test.o
/opt/homebrew/opt/binutils/bin/readelf -a test/build/test.o