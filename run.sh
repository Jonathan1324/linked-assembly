#!/bin/sh

if [ "$1" = "-c" ]; then
    make clean

    chmod +x examples/tests/clean.sh

    cd examples/tests
    sh clean.sh $@
    cd ../..
else
    make all
    chmod +x build/assembly/assembly
    chmod +x examples/tests/test.sh
    chmod +x examples/tests/debug.sh

    cd examples/tests
    sh test.sh $@
    cd ../..
fi