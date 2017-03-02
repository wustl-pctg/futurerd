#include "common.h"
#include <future.hpp>
#include <futurerd.hpp>

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
  futurerd::set_policy(futurerd::CONTINUE);

  create_future(int, f, fib, 10);

  assert(f.get() == 55);
  assert(futurerd::num_races() == 0);

  return 0;
}
