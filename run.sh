#!/bin/sh

if [ "$1" = "-c" ]; then
    chmod +x tests/clean.sh

    cd tests
    sh clean.sh $@
    cd ..
else
    make DEBUG=1
    chmod +x build/assembler/assembler
    chmod +x tests/test.sh
    chmod +x tests/debug.sh

    cd tests
    sh test.sh $@
    cd ..
fi