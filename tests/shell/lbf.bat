cd tests\lbf\srcs

..\..\..\dist\bin\lbf %* main.xbf
if not exist build (
    mkdir build
)
..\..\..\dist\bin\lbf -c main.xbf -o build/out.bf

cd ..\..\..
