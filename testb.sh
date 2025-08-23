python3 -m ci.ci --no-test

cd tests/buildtool/envs/test

../../../../dist/bin/labt $@

#build/project2/out/main

cd ../../../..
