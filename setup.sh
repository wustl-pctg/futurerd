#!/bin/sh
# Script to setup everything, for reproducible results.

# Setup compiler (LLVM)
#./build-llvm-linux.sh

# Setup runtime system
mkdir -p build

#git clone https://gitlab.com/wustl-pctg/cilkplus-rts runtime
cd runtime
autoreconf -i
./configure --prefix=$HOME/devel/futurerd/build
make -j
make install
