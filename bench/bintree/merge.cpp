// Repeatedly merge binary trees.
#include <chrono>

#include "bintree.hpp"
#include "../util/util.hpp"
#include "../util/getoptions.hpp"

using tkey_t = bintree::key_t;

static inline tkey_t randkey(tkey_t key_max) { return rand() % key_max; }

bintree* prepare(size_t size, tkey_t key_max) {
  bintree* t = new bintree();
  for (int i = 0; i < size; ++i) t->insert(randkey(key_max));
  assert(t->validate());
  return t;
}

// size of first tree, size of second tree, max key to use
const char* specifiers[] = {"-s1", "-s2", "-kmax", "-c"};
int opt_types[] = { INTARG, INTARG, INTARG, BOOLARG };

int main(int argc, char* argv[]) {
  size_t t1_size = 4096;
  size_t t2_size = 2048;
  tkey_t key_max = 4096 * 4;
  int check = 0;

  ensure_serial_execution();
  get_options(argc, argv, specifiers, opt_types,
              &t1_size, &t2_size, &key_max, &check);

  if( t1_size < t2_size ) { // always make t2, the one we split, smaller
    size_t tmp = t1_size;
    t1_size = t2_size;
    t2_size = tmp;
  }

  int *t1_key_counts = nullptr, *t2_key_counts = nullptr;


  // Prep the trees
  bintree* t1 = prepare(t1_size, key_max);
  bintree* t2 = prepare(t2_size, key_max);

  if(check) {
    t1_key_counts = new int[key_max]{0};
    t2_key_counts = new int[key_max]{0};
    t1->get_key_counts(t1_key_counts, key_max);
    t2->get_key_counts(t2_key_counts, key_max);

    for(int i = 0; i < key_max; i++) {
      t1_key_counts[i] += t2_key_counts[i];
      t2_key_counts[i] = 0; // reset t2_key_counts;
    }
  }

  auto start = std::chrono::steady_clock::now();
  t1->merge(t2);
  auto end = std::chrono::steady_clock::now();

  // Don't need to time this. Presumably the client would need to
  // actually use the result sometime, at which point the futures
  // would be "touched".
  t1->replace_all();

  if(check) {
    t1->validate();
    t1->get_key_counts(t2_key_counts, key_max);
    // make sure that we got all the keys correctly
    for(int i = 0; i < key_max; i++) {
      assert(t1_key_counts[i] == t2_key_counts[i]);
    }
    printf("Check passed.\n");

    delete[] t1_key_counts;
    delete[] t2_key_counts;
  }

  auto time = std::chrono::duration <double, std::milli> (end-start).count();
  printf("Benchmark time: %f ms\n", time);

  delete t1; // The merge takes care of t2

  return 0;
}
