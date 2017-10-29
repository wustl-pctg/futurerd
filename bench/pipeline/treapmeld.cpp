// Repeatedly merge binary trees.
#include <cstdlib>

#include "ds/treap.hpp"
#include "../common/timer.hpp"
#include "../common/options.hpp"

using key_t = int;
size_t g_key_range_left = 0;
size_t g_key_range_right = 42;

inline key_t randkey() {
  return g_key_range_left + rand() %
    (g_key_range_right - g_key_range_left + 1);
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
  // range of priorites?
  size_t prio_range_left = 0;
  size_t prio_range_right = 42;
  // range of keys?

  for (size_t i = 0; i < num_iter; ++i) {
    // Prep the trees
    treap* t1 = new treap(prio_range_left, prio_range_right);
    prepare(t1, t1_size);
    treap* t2 = new treap(prio_range_left, prio_range_right);
    prepare(t2, t2_size);

    treap::meld(t1, t2);

    // cleanup
    delete t1; delete 2;
  }
  
  return 0;
}
