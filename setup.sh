#!/bin/sh
# Script to setup everything, for reproducible results.

# Setup compiler (LLVM)
#./build-llvm-linux.sh

# Setup runtime system
git clone https://gitlab.com/wustl-pctg/cilkplus-rts runtime
