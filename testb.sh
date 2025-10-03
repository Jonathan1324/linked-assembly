python3 -m ci.ci --no-test

cd tests/buildtool/envs/wasm

../../../../dist/bin/lbt $@

cd ../../../..
