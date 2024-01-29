#!/bin/sh

for perft in $(ls *.perft); do
    echo -ne "\n$perft" >> benchmark_python;
    { time python perft.py $perft > "$(basename "$perft" .perft).out"; }
        2>> benchmark_python;
done
