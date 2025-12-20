#!/bin/sh

python3 -m ci.ci

if [ $# -lt 1 ]; then
    echo "Usage: $0 <name> <args>"
    exit 1
fi

name="$1"
shift

script="./tests_sh/${name}.sh"

if [ ! -x "$script" ]; then
    echo "Error: $script not found or not executable"
    exit 2
fi

/bin/sh "$script" "$@"
