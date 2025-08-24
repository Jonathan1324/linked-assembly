python3 -m ci.ci --no-test

cd tests/lbf

../../dist/bin/lbf $@ main.bf

cd ../..
