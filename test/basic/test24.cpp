// for loop with local variables and futures, no race
#include "common.h"
#include <future.hpp>

using Fut = cilk::future<int>;

int foo(int& shared, int x) { return shared + x; }

int main() {
  TEST_SETUP();

  int n = 2; // 128
  int shared = 42;
  auto farray = (Fut*) malloc(sizeof(Fut) * n);
  
  for(int i{0}; i < n; ++i) {

    // The second iteration we write to x, race with the read below
    int x = 57;

    // The read of x occurs AFTER the future has been "launched", in
    // the new strand
    reuse_future(int, &farray[i], foo, shared, x);
  }

  for (int i = 0; i < n; i++) {
    farray[i].get();
  }
  
           
  free(farray);
  assert_detected(0);

  TEST_TEARDOWN();
  return 0;
}
