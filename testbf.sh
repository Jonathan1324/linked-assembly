python3 -m ci.ci --no-test

cd tests/lbf/env

../../../dist/bin/lbf $@ main.bf

cd ../../..
