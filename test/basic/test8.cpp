// Simple race in a fork-join program
#include "common.h"
#include <future.hpp>

int g_shared = 0;

int fib(int n)
{
  if (n < 2) return n;

  // Clang (erroneously) thinks x could be uninitialized
  #pragma GCC diagnostic ignored "-Wuninitialized"
  int x = spawn fib(n - 1);
  int y = fib(n - 2);
  sync;

  g_shared++;

  return x + y;
}

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();

  cilk_async(int, f, fib, 10);

  assert(f->get() == 55);
  if(futurerd_num_races() == 0) {
    fprintf(stderr, "num_race should have > 0 but it's %zu.\n", futurerd_num_races());
  }
  assert(futurerd_num_races() > 0);

  TEST_TEARDOWN();
  FUTURE_EPILOG();
  return 0;
}
