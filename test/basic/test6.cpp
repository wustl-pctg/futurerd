// Access to shared variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int foo() { return 42; }

int bar(cilk::future<int>* f, int& shared) {
  int x = f->get();
  return shared - x;
}

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();
  
  int shared = 0;
  
  cilk_async(int, f, foo);
  cilk_async(int, g, bar, f, shared);
  shared = 57;
  int x = g->get();

  assert(x == 15 || x == -42);
  assert(futurerd_num_races() == 1);

  TEST_TEARDOWN();
  FUTURE_EPILOG();

  return 0;
}
