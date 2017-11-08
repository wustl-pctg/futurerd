#include "common.h"
#include <future.hpp>

int fib(int n)
{
  if (n < 2) return n;

  // Clang (erroneously) thinks x could be uninitialized
  #pragma GCC diagnostic ignored "-Wuninitialized"
  int x = spawn fib(n - 1);
  int y = fib(n - 2);
  sync;

  return x + y;
}

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();

  cilk_async(int, f, fib, 10);
  assert(f->get() == 55);

  size_t num_races = futurerd_num_races();
  if(num_races != 0) { 
    fprintf(stderr, "Should not have detected race, but num races = %zu.\n", num_races);
  }
  assert(futurerd_num_races() == 0);

  TEST_TEARDOWN();
  FUTURE_EPILOG();
  return 0;
}

