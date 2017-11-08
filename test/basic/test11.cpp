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
  size_t num_races = futurerd_num_races();
  if(num_races != 1) {
    fprintf(stderr, "num_race should have been 1 but it's %zu.\n", num_races);
  }
  assert(futurerd_num_races() == 1);

  TEST_TEARDOWN();
  
  return 0;
}
