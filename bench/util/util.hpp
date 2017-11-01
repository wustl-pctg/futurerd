#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Ensure that we run serially
static void ensure_serial_execution(void) {
  // assert(1 == __cilkrts_get_nworkers());
  fprintf(stderr, "Forcing CILK_NWORKERS=1.\n");
  char *e = getenv("CILK_NWORKERS");
  if (!e || 0!=strcmp(e, "1")) {
    // fprintf(err_io, "Setting CILK_NWORKERS to be 1\n");
    if( setenv("CILK_NWORKERS", "1", 1) ) {
      fprintf(stderr, "Error setting CILK_NWORKERS to be 1\n");
      exit(1);
    }
  }
}

static void gen_rand_string(char * s, int s_length, int range) {
  for(int i = 0; i < s_length; ++i ) {
    s[i] = (char)(rand() % range + 97);
  }
}

#endif // __UTIL_HPP__
