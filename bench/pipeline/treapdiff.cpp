// Repeatedly merge binary trees.
#include <cstdlib>

#include "ds/treap.hpp"
#include "../common/timer.hpp"
#include "../common/options.hpp"

using key_t = int;

inline key_t randkey(key_t range_left, key_t range_right) {
  return range_left + rand() % (range_right - range_left + 1);
}

void prepare(treap* t, size_t size) {
  for (int i = 0; i < size; ++i)
    t.insert(randkey());

}

int main(int argc, char* argv[]) {

  // Get options
  size_t t1_size = 42;
  size_t t2_size = 42;
  size_t num_iter = 42;
  // range of keys?
  size_t range_left = 0;
  size_t range_right = 42;

  for (size_t i = 0; i < num_iter; ++i) {
    // Prep the trees
    treap* t1 = new treap();
    prepare(t1, t1_size);
    treap* t2 = new treap();
    prepare(t2, t2_size);

    treap::diff(t1, t2);

    // cleanup
    delete t1; delete 2;
  }
  
  return 0;
}
