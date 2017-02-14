// Access to global variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>
#include <futurerd.hpp>

int g_shared = 0;

void foo(cilk::future<int>& f) {
  f.put(42);
  g_shared = 57;
}

int bar(cilk::future<int>& f) {
  int x = f.get();
  return g_shared - x;
}


int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::CONTINUE);
  cilk::future<int> f;
  f.start();
  spawn foo(f);
  int x = bar(f);
  sync;

  assert(x == 15 || x == -42);
  assert(futurerd::num_races() == 1);

  return 0;
}
