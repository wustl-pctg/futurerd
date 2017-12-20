// for loop with local variables and futures, no race
#include "common.h"
#include <future.hpp>

int foo(int& shared, int x) { return shared + x; }

int main() {
  TEST_SETUP();

  int n = 128;
  int shared = 42;
  auto farray = (cilk::future<int>*)
    malloc(sizeof(cilk::future<int>) * n);

  // #pragma cilk grainsize = 1
  CILKFOR_BEGIN;
  cilk_for(int i = 0; i < n; ++i) {
    CILKFOR_ITER_BEGIN;
    int x = 57;
    auto f = &(farray[i]);
    reuse_future(int, f, foo, shared, x);
    CILKFOR_ITER_END;
  } CILKFOR_END;

  cilk_for(int i = 0; i < n; ++i)
    farray[i].get();
           
  free(farray);
  assert_detected(0);

  TEST_TEARDOWN();
  return 0;
}
