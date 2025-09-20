python3 -m ci.ci --no-test

cd tests/lbf/srcs

../../dist/bin/lbf $@ main.bf
../../dist/bin/lbf -c main.xbf -o build/bf

cd ../../..
