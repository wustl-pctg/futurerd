// Common definitions for the tests
#include <cilk/cilk.h>
#define RACE_DETECT 1
#include <rd.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#define spawn cilk_spawn
#define sync cilk_sync

// setup for a test
#define TEST_SETUP()                      \
  futurerd_set_policy(RD_SILENT); \
  
// Don't actually need this for now (in C++)
#define TEST_TEARDOWN()

// Exact assertion
static inline void assert_detected(size_t n) {
  size_t num_races = futurerd_num_races();
  if (num_races != n) {
    fprintf(stderr, "Expected %zu races, found %zu.\n", n, num_races);
    abort();
  }
}

static inline void assert_atleast(size_t n) {
    size_t num_races = futurerd_num_races();
    if (num_races < n) {
      fprintf(stderr, "Expected at least %zu races, found %zu\n", n, num_races);
      abort();
    }
}
