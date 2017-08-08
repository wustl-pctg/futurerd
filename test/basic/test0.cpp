// Regression test: hopefully we didn't mess anything up.
#include "common.h"
#include <cassert>
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
  TEST_SETUP();
  int result = fib(10);
  assert(result == 55);
  assert(futurerd::num_races() == 0);
  TEST_TEARDOWN();
  return 0;
}
