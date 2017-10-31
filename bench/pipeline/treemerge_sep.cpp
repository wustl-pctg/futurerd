// Repeatedly merge binary trees.
#include <cstdlib>

#include "ds/bintree2.hpp"
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
  return t;
}

int main(int argc, char* argv[]) {

  // Get options
  size_t t1_size = 42;
  size_t t2_size = 42;
  size_t num_iter = 1;
  // range of keys?

  for (size_t i = 0; i < num_iter; ++i) {
    // Prep the trees
    bintree* t1 = prepare(t1_size);
    bintree* t2 = prepare(t2_size);

    t1->merge(t2);

    // cleanup
    delete t1; delete t2;
  }
  
  return 0;
}
