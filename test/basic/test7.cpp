// Access to global variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int g_shared = 0;

int foo(cilk::future<int>& f) {
  f.finish(42);
  g_shared = 57;
  return 0;
}

int bar(cilk::future<int>& f) {
  int y = g_shared;
  int x = f.get();
  return y - x;
}


int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::CONTINUE);

  create_future2(int, f, foo, f);
  int x = bar(f);

  assert(x == 15 || x == -42);
  assert(futurerd::num_races() == 1);

  return 0;
}
