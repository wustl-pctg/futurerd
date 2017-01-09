#include <iostream>
#include <cstdlib>

#include <cilk/cilk.h>
#define spawn cilk_spawn
#define sync cilk_sync
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

void wrapper(cilk::future<int>& f, int n)
{
  f.start();
  f.put(fib(n));
}

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::CONTINUE);
  int n = (argc != 2) ? 10 : atoi(argv[1]);
  
  cilk::future<int> f;
  wrapper(f, n);

  std::cout << "fib(" << n << ") = " << f.get() << std::endl;
  assert(futurerd::num_races() == 0);

  return 0;
}
