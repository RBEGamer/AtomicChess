#!/usr/bin/bash

wai=$(dirname $(readlink -f "$0")) # Current script directory
cd ${wai}/../build/tests/
wget -O  sf.tar.gz https://github.com/official-stockfish/Stockfish/archive/refs/tags/sf_14.1.tar.gz
mkdir -p sf_src && tar -xf sf.tar.gz -C sf_src --strip-components 1
cd sf_src/src && make build ARCH=x86-64-modern && mv stockfish ./../../stockfish_engine