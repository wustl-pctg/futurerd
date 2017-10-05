// Common definitions for the tests
#include <cilk/cilk.h>
#define RACE_DETECT 1
#include <rd.h>

#define spawn cilk_spawn
#define sync cilk_sync

// setup for a test
#define TEST_SETUP()                      \
  futurerd_set_policy(RD_SILENT); \
  
// Don't actually need this for now (in C++)
#define TEST_TEARDOWN()
