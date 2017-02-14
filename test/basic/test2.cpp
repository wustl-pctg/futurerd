// Very simple use of futures
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>
#include <futurerd.hpp>

void foo(cilk::future<int>& f) { f.put(42); }
int bar(cilk::future<int>& f) { return f.get(); }

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::CONTINUE);

  cilk::future<int> f;
  f.start();
  spawn foo(f);
  int x = bar(f);
  sync;

  assert(x == 42);
  assert(futurerd::num_races() == 0);

  return 0;
}
