// Access to global variable -- no race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int g_shared = 0;

int foo(cilk::future<int>& f) {
  g_shared = 57;
  return 42;
}

int bar(cilk::future<int>& f) {
  int x = f.get();
  return g_shared - x;
}

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::CONTINUE);

  create_future(int, f, foo, f);
  int x = bar(f);

  assert(x == 15);
  assert(futurerd::num_races() == 0);

  return 0;
}
