// Access to shared variable -- race
#include <iostream>
#include <cassert>

#include "common.h"
#include <future.hpp>
#include <futurerd.hpp>

void foo(cilk::future<int>& f, int& shared) {
  f.put(42);
  shared = 57;
}

int bar(cilk::future<int>& f, int& shared) {
  int x = f.get();
  return shared - x;
}

int main(int argc, char* argv[])
{
  futurerd::set_policy(futurerd::CONTINUE);
  int shared = 0;
  cilk::future<int> f; f.start();
  spawn foo(f, shared);
  int x = bar(f, shared);
  sync;

  assert(x == 15 || x == -42);
  assert(futurerd::num_races() == 1);
  
  return 0;
}
