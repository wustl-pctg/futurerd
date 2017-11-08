// Access to global variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int g_shared = 0;

int bar(cilk::future<int>* f) {
  int y = g_shared;
  int x = f->get();
  return y - x;
}

int foo() { return 42; }

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();

  cilk_async(int, f, foo);
  cilk_async(int, g, bar, f);
  g_shared = 57;
  int x = g->get();

  assert(x == 15 || x == -42);
  if(futurerd_num_races() != 1) {
    fprintf(stderr, "num_race should have been 1 but it's %zu.\n", futurerd_num_races());
  }
  assert(futurerd_num_races() == 1);
  
  TEST_TEARDOWN();
  FUTURE_EPILOG();
  return 0;
}
