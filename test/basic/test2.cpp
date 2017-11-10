// Very simple use of futures
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

int foo(cilk::future<int>* f) { return 42; }
int bar(cilk::future<int>* f) { return f->get(); }

int main(int argc, char* argv[])
{
  FUTURE_PROLOG();
  TEST_SETUP();

  cilk_async(int, f, foo, f);
  int x = bar(f);

  assert(x == 42);
  assert_detected(0);

  TEST_TEARDOWN();
  FUTURE_EPILOG();
  
  return 0;
}
