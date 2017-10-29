// Repeatedly merge binary trees.
#include <cstdlib>

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#include "ds/bintree3.hpp"
#include "../common/timer.hpp"
#include "../common/options.hpp"

using key_t = int;
key_t g_key_min = 0;
key_t g_key_max = 32;

inline key_t randkey() {
  return g_key_min + rand() % (g_key_max - g_key_min + 1);
}

bintree* prepare(size_t size) {
  bintree* t = new bintree();
  for (int i = 0; i < size; ++i)
    t->insert(randkey());
  t->validate();
  return t;
}

int main(int argc, char* argv[]) {

  __cilkrts_set_param("nworkers", "1"); 

  // Get options
  size_t t1_size = 4096*4;
  size_t t2_size = 4096*4;
  size_t num_iter = 2;
  // range of keys?

  for (size_t i = 0; i < num_iter; ++i) {
    // Prep the trees
    bintree* t1 = prepare(t1_size);
    //printf("t1: "); t1->print_keys();
    bintree* t2 = prepare(t2_size);
    //printf("t2: "); t2->print_keys();

    t1->merge(t2);
    //printf("merged: "); t1->print_keys();
    t1->validate();

    // cleanup
    delete t1; delete t2;
  }
  
  return 0;
}
