#!/bin/sh

if [ "$1" = "-c" ]; then
    chmod +x tests/clean.sh

    cd tests
    sh clean.sh $@
    cd ..
elif [ "$1" = "-m" ]; then
    make DEBUG=1
else
    if [ "$2" != "-no-make" ]; then
        make DEBUG=1
    fi
    chmod +x bin/assembler
    chmod +x bin/linker
    chmod +x tests/test.sh
    chmod +x tests/debug.sh

    cd tests
    sh test.sh $@
    cd ..
fi