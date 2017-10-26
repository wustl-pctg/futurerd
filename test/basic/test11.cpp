// Fork-join access to global function. Function that is spawned does
// not itself spawn.
#include <iostream>
#include <cassert>

#include "common.h"

int g_shared = 0;

void foo() { g_shared = 57; }

int bar() {
  int y = g_shared;
  return y;
}


int main(int argc, char* argv[])
{
  TEST_SETUP();

  spawn foo();
  int x = bar();
  sync;

  assert(x == 0 || x == 57);
  assert(futurerd_num_races() == 1);

  TEST_TEARDOWN();
  
  return 0;
}
