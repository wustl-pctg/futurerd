// Common definitions for the tests
#include <cilk/cilk.h>
#define spawn cilk_spawn
#define sync cilk_sync

// setup for a test
#define TEST_SETUP()                      \
  futurerd::set_policy(futurerd::SILENT); \
  
// Don't actually need this for now (in C++)
#define TEST_TEARDOWN()
