// Access to shared variable -- no race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>
#include <futurerd.hpp>

int foo(cilk::future<int>& f, int& shared) {
  shared = 57;
  return 42;
}

int bar(cilk::future<int>& f, int& shared) {
  int x = f.get();
  return shared - x;
}

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::CONTINUE);
  
  int shared = 0;
  create_future(int, f, foo, f, shared);
  int x = bar(f, shared);

  assert(x == 15);
  assert(futurerd::num_races() == 0);
  
  return 0;
}
