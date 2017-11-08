// Access to shared variable -- no race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int foo(cilk::future<int>* f, int& shared) {
  shared = 57;
  return 42;
}

int bar(cilk::future<int>* f, int& shared) {
  int x = f->get();
  return shared - x;
}

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();
  
  int shared = 0;
  cilk_async(int, f, foo, f, shared);
  //create_future(int, f, foo, f, shared);
  int x = bar(f, shared);

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
