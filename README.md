# Race Detection for Future-Parallel Compuations

A project to automatically detect determinacy races on-the-fly in
programs that use futures. Users should use the futures provided by
this project, not the class of future objects provided with C++11.

Currently, this system only detects futures while running
sequentially. We have plans to parallelize our race detection
algorithm.

## Using

### Compiling the tool

1. First we need to build the compiler. If using link-time
   optimization, set `BINUTILS_PLUGIN_DIR` in `build-llvm-linux.sh` to
   the directory where `plugin-api.h` is located. After this (or if
   you're not using LTO), run `build-llvm-linux.sh`.
   
2.

### Compiling programs

## TODO
test
test
test
