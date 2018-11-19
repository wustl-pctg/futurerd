#!/bin/sh
# Script to setup everything, for reproducible results.
BASE=$(pwd)
COMPDIR=$BASE/llvm-cilk/bin

# Setup compiler (LLVM)
#./build-llvm-linux.sh

#git clone https://github.com/wsmoses/Tapir-Meta tapir

# Setup runtime system
mkdir -p build

git clone -b futurerd https://gitlab.com/wustl-pctg/cilkplus-rts runtime
cd runtime
autoreconf -i
# FLAGS="-g -O0 -fcilk-no-inline -fcilktool"
# ./configure --prefix=$BASE/build CC=$COMPDIR/clang CXX=$COMPDIR/clang++ CFLAGS=$FLAGS CXXFLAGS=$FLAGS
# make -j
# make install
./remake.sh
cd -
