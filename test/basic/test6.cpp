// Access to shared variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>

void foo(cilk::future<int>& f, int& shared) {
  f.put(42);
  shared = 57;
  return;
}

int bar(cilk::future<int>& f, int& shared) {
  int x = f.get();
  return shared - x;
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
