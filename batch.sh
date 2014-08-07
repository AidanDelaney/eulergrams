#!/bin/sh

for json in tests/data/*.json; do
    LD_LIBRARY_PATH=/usr/local/lib src/eulergrams --json
done
