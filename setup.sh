#!/bin/sh
# Script to setup everything, for reproducible results.
BASE=$(pwd)
COMPDIR=$BASE/llvm-cilk/bin

# Setup compiler (LLVM)
#./build-llvm-linux.sh

# Setup runtime system
mkdir -p build

#git clone https://gitlab.com/wustl-pctg/cilkplus-rts runtime
cd runtime
FLAGS="-g -O0 -fcilk-no-inline -fcilktool"
autoreconf -i
./configure --prefix=$BASE/build CC=$COMPDIR/clang CXX=$COMPDIR/clang++ CFLAGS=$FLAGS CXXFLAGS=$FLAGS
make -j
make install
