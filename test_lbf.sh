python3 -m ci.ci --no-test

cd tests/lbf/srcs

../../../dist/bin/lbf $@ main.xbf
mkdir -p build
../../../dist/bin/lbf -c main.xbf -o build/out.bf

cd ../../..
