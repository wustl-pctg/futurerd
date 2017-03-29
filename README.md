# Race Detection for Future-Parallel Compuations

A project to automatically detect determinacy races on-the-fly in
programs that use futures. Users should use the futures provided by
this project, not the class of future objects provided with C++11.

The master branch detects races while running sequentially. Other
branches will detect races running in parallel, as well as detecting
races when using futures in a structured way.

## Using

### Compiling the tool

1. First we need to build the compiler. If using link-time
   optimization, set `BINUTILS_PLUGIN_DIR` in `build-llvm-linux.sh` to
   the directory where `plugin-api.h` is located. After this (or if
   you're not using LTO), run `build-llvm-linux.sh`.
   
2.

### Compiling programs

## TODO

* Futures should really be a part of the runtime, like in
  <cilk/future.hpp>, with added cilktool calls. Then this source tree
  just implements the cilktool calls. The problem is that I think we
  need to store extra information with each future. Either we need to
  embed this info in the future class, which means either 
  
  (1) the tool can only be turned on statically, while compiling the
  user's program, or
  
  (2) We need to hash each future to a separate location for the extra
  information, which adds overhead.
  
* Find a way to make this (and CRacer) work with PIN.

* Setup Doxygen to collect various tags:
  * Bug
  * Refactor (enhancement?)
  * Features (enhancement?)
  * Question (as in, why is this code and what is it doing?)
  
* Get some benchmarks!
  
* Setup automatic generation of a performance report
  * For now, just run some benchmarks and generate a few plots
  * Later, turn on profiling and generate a bunch of data
  * Try to display this is an R notebook or something similar
