// Access to global variable -- race
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
  int y = g_shared;
  int x = f->get();
  return y - x;
}


int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();

  //cilk_async(int, f, foo, f);
  // Writing this manually to debug
  race_detector::disable_checking();
  auto f = new cilk::future<int>();
  race_detector::enable_checking();
  f->finish(foo(f));
  int x = bar(f);

  assert(x == 15 || x == -42);
  std::cout << "Found " << futurerd_num_races() << " races." << std::endl;
  assert(futurerd_num_races() == 1);

  TEST_TEARDOWN();
  FUTURE_EPILOG();
  
  return 0;
}
