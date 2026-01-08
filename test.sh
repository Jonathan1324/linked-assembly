#!/bin/sh

set -e
python3 -m ci.ci

if [ $# -lt 1 ]; then
    echo "Usage: $0 <name> <args>"
    exit 1
fi

name="$1"
shift

script="./tests/shell/${name}.sh"

if [ ! -f "$script" ]; then
    echo "Error: $script not found"
    exit 2
fi

sh "$script" "$@"
