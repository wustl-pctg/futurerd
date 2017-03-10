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
  futurerd::set_policy(futurerd::SILENT);
  futurerd::set_loc((void*)&g_shared);

  create_future(int, f, fib, 10);

  assert(f.get() == 55);
  assert(futurerd::num_races() > 0);

  return 0;
}
