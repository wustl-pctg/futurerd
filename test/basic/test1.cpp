#include <iostream>
#include <unistd.h> // sleep
#include <cstdlib>

#include <cilk/cilk.h>
#define spawn cilk_spawn
#define sync cilk_sync
#include <future.hpp>

int fib(int n)
{
  if (n < 2) return n;

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
  if (argc != 1) {
    std::cerr << "Usage: "
              << argv[0] << " <n> " << std::endl;
    std::exit(1);
  }
  int n = atoi(argv[1]);
  
  cilk::future<int> f;
  wrapper(f, n);

  std::cout << "fib(" << n << ") = " << f.get() << std::endl;
  return 0;
}
