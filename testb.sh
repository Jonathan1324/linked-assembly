python3 -m ci.ci --no-test

cd tests/buildtool/envs/test

../../../../dist/bin/buildtool $@

build/out/main

cd ../../../..
