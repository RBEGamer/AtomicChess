#!/bin/bash

rm -R CMakeFiles/ CMakeCache.txt
make clean



cmake .
make
