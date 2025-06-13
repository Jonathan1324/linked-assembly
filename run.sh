#!/bin/sh

if [ "$1" = "-c" ]; then
    chmod +x tests/clean.sh

    cd tests
    sh clean.sh $@
    cd ..
else
    make all
    chmod +x build/assembly/assembly
    chmod +x tests/test.sh
    chmod +x tests/debug.sh

    cd tests
    sh test.sh $@
    cd ..
fi