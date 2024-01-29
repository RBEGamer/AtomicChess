#!/usr/bin/bash

wai=$(dirname $(readlink -f "$0")) # Current script directory
cd ${wai}/../build/tests/
wget -O  berserk_src.tar.gz https://github.com/jhonnold/berserk/archive/refs/tags/8.5.1.tar.gz
mkdir -p berserk_src && tar -xf berserk_src.tar.gz -C berserk_src --strip-components 1
cd berserk_src/src && make && mv berserk ./../../berserk_engine

