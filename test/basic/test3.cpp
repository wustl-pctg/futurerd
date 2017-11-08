// Access to global variable -- no race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int g_shared = 0;

int foo(cilk::future<int>* f) {
  g_shared = 57;
  return 42;
}

int bar(cilk::future<int>* f) {
  int x = f->get();
  return g_shared - x;
}

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();

  cilk_async(int, f, foo, f);
  int x = bar(f);

  assert(x == 15);
  size_t num_races = futurerd_num_races();
  if(num_races != 0) { 
    fprintf(stderr, "Should not have detected race, but num races = %zu.\n", num_races);
  }
  assert(futurerd_num_races() == 0);

  TEST_TEARDOWN();
  FUTURE_EPILOG();
  
  return 0;
}
