// Very simple use of futures
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int foo(cilk::future<int>& f) { return 42; }
int bar(cilk::future<int>& f) { return f.get(); }

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::DetectPolicy::SILENT);

  // cilk::future<int> f = foo(f);
  // int x = bar(f);

  create_future(int, f, foo, f);
  int x = bar(f);

  assert(x == 42);
  assert(futurerd::num_races() == 0);

  return 0;
}
