mkdir -p build
../build/assembly/assembly main.asm -o build/test.o \
    --arch x86 \
    -m32 \
    --format elf \
    --debug