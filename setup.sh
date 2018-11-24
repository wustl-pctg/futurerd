#!/bin/sh
# Script to setup everything, for reproducible results.
BASE=$(pwd)
COMPDIR=$BASE/llvm-cilk/bin

# Setup compiler (LLVM)
./build-llvm-linux.sh

#git clone https://github.com/wsmoses/Tapir-Meta tapir

# Setup runtime system
mkdir -p build

git clone -b futurerd https://gitlab.com/wustl-pctg/cilkplus-rts runtime
# cd runtime
# autoreconf -i
# ./remake.sh opt pre
# cd -

cd bench/pipeline/dedup/inputs
wget http://parsec.cs.princeton.edu/download/3.0/parsec-3.0-input-native.tar.gz
wget http://parsec.cs.princeton.edu/download/3.0/parsec-3.0-input-sim.tar.gz
wget http://parsec.cs.princeton.edu/download/3.0/parsec-3.0-core.tar.gz
tar vxzf parsec-3.0-input-native.tar.gz
tar vxzf parsec-3.0-input-sim.tar.gz
tar vxzf parsec-3.0-core.tar.gz
mv parsec-3.0/pkgs/kernels/dedup/inputs/*.tar .
./unpack.sh
cd -
