#!/bin/sh

python3 -m ci.ci

cd tests/lfs/envs/test

mkdir -p build

../../../../dist/bin/lfs -o build/out.img $@

cd ../../../..