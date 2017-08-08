// Access to shared variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

// Originally I wrote it like this, but we don't currently support a
// separate "put" operation
#if 0
void foo(cilk::future<int>& f, int& shared) {
  f.put(42);
  shared = 57;
  return;
}

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::DetectPolicy::SILENT);
  
  int shared = 0;
  futurerd::set_loc((void*)&shared);
  create_future2(int, f, foo, f, shared);
  int x = bar(f, shared);

  assert(x == 15 || x == -42);
  assert(futurerd::num_races() == 1);
  
  return 0;
}
#endif

int foo() { return 42; }

int bar(cilk::future<int>& f, int& shared) {
  int x = f.get();
  return shared - x;
}

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();
  
  int shared = 0;
  
  create_future(int, f, foo);
  create_future(int, g, bar, f, shared);
  shared = 57;
  int x = g.get();

  assert(x == 15 || x == -42);
  assert(futurerd::num_races() == 1);

  TEST_TEARDOWN();
  FUTURE_EPILOG();

  return 0;
}
