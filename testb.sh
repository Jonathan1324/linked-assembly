python3 -m ci.ci --no-test

cd tests/buildtool/envs/lib

../../../../dist/bin/lbt $@

#build/project2/out/main

cd ../../../..
