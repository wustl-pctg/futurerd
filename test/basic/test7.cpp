// Access to global variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int g_shared = 0;

// Originally I wrote it like this, but we don't currently support a
// separate "put" operation
#if 0
int foo(cilk::future<int>& f) {
  f.finish(42);
  g_shared = 57;
  return 0;
}

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::DetectPolicy::SILENT);
  futurerd::set_loc((void*)&g_shared);

  create_future2(int, f, foo, f);
  int x = bar(f);

  assert(x == 15 || x == -42);
  assert(futurerd::num_races() == 1);

  return 0;
}
#endif

int bar(cilk::future<int>& f) {
  int y = g_shared;
  int x = f.get();
  return y - x;
}

int foo() { return 42; }

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();

  create_future(int, f, foo);
  create_future(int, g, bar, f);
  g_shared = 57;
  int x = g.get();

  assert(x == 15 || x == -42);
  assert(futurerd::num_races() == 1);
  
  TEST_TEARDOWN();
  FUTURE_EPILOG();
  return 0;
}
