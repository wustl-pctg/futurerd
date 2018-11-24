# Race Detection for Future-Parallel Computations

A project to automatically detect determinacy races on-the-fly in
programs that use futures. Users should use the futures provided by
this project, not the class of future objects provided with C++11.


We can detect races for futures used in two different ways:

* "Structured" futures can only be touched (get()) once, and this
      must occur sequentially after the future was created.
* Futures that are "forward-pointing": the creator strand of each
  future executes before every getter strand of that future in the
  depth-first eager execution. This restriction is only because any
  program that does not use forward-pointing futures could
  deadlock. Note: I previously used the name "nonblocking" (a poor
  choice) for these futures; you may see this name throughout the code
  until it is cleaned up.

Currently, this system only detects futures while running
sequentially. We have plans to parallelize our race detection
algorithms.

## License

Unless otherwise specified, the code in this repository is distributed
under the MIT license. All code from external projects -- the Cilk
Plus runtime, some benchmarks, and the LLVM compiler infrastructure --
is separately licensed.

## Using

### Compiling the tool

1. First use the `setup.sh` script to build the compiler and download
   the `dedup` datasets. If using link-time optimization (recommended
   to replicate results), set `BINUTILS_PLUGIN_DIR` in
   `build-llvm-linux.sh` to the directory where `plugin-api.h` is
   located. After this (or if you're not using LTO), run
   `build-llvm-linux.sh`. Also make sure that the system linker (`ld`)
   points to GNU `gold`.

2. If replicating results, run `bench/run.sh`, which will compile all
   the remaining pieces and run all benchmarks. Otherwise you can run
   `make mode=release rdalg=ALG` in the `src` directory, where `ALG`
   can be either `structured` (for MultiBags) or `nonblock` (for
   MultiBags+).

### Compiling your own programs

Compile your programs with our compiler, using the `-fcilktool
-fsanitize=thread` flags, and link with the library. Now races will be
detected, assuming you're not using locks or any form of
synchronization other than spawn/sync and futures.

### API Options

(TODO)

## TODO

* Take better shadow memory from cilksan/cracer

* Port cilksan tests

* Refactor treap merge/split
  * Some kind of "future_ptr" wrapper similar to smart pointers may make this easier

* Implement other benchmarks: other data structures from "pipelining
  with futures", plus rician denoising, etc.

* Change how RD_policy is implemented. Really the right thing to do is read an environment variable that sets the policy. The policy determines what to do with a race: 
  * abort -- quit at the first race
  * count -- just count the number of races and print the number at
    the end
  * print -- immediately print (to stderr) information about any race
    and continue (also count)
  * log -- record information about each race and print a report at the end
  * optionally we can support an RDLOGFILE environment variable which
    prints/logs race info to a separate file

* Change tool initialization: the tool initializes as if it was a
  completely dynamic tool. But since we're relying on compiler
  instrumentation anyway, this is unnecessary and makes some code more
  complicated. We should just use existing compiler features to make
  sure initialization happens before main.

* Futures should really be a part of the runtime, as in
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
  * For debugging, it would be nice to log certain events. Consider
  using a simple logging library, such as sclog4c or
  especially [zlog](https://github.com/HardySimpson/zlog).

* Setup automatic generation of a performance report
  * For now, just run some benchmarks and generate a few plots
  * Later, turn on profiling and generate a bunch of data
  * Try to display this is an R notebook or something similar

* One cool features for a practical implementation would be having a
  .then() method for futures. This attaches the function to be called
  when the future finishes, and returns a future for this function. I
  guess this is really just equivalent to "spawning" a future with a
  function that first waits for the original function.
