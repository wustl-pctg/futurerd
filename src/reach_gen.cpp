#include "reach_gen.hpp"
#include <cassert>
#include <cstdlib>

#ifdef STATS
#include <iostream>
#endif

using namespace reach;
using node = general::node;

// The 0th node is fake, for representing nodes in R
general::general() { add_node(); }

general::~general() {
  // Stats
#ifdef STATS
  std::cerr << data.size() << " nodes total." << std::endl;

  size_t max_in_count = 0UL, min_in_count = -1UL, sum_in_count = 0;
  size_t max_reach_count = 0UL, min_reach_count = -1UL, sum_reach_count = 0;
  size_t max_block_count = 0UL, min_block_count = -1UL, sum_block_count = 0;
  for (int i = 1; i < data.size(); ++i) {
    bitset& ba = data[i];

    if (ba.in_count > max_in_count) max_in_count = ba.in_count;
    if (ba.in_count < min_in_count) min_in_count = ba.in_count;
    sum_in_count += ba.in_count;

    if (ba.reach_count > max_reach_count) max_reach_count = ba.reach_count;
    if (ba.reach_count < min_reach_count) min_reach_count = ba.reach_count;
    sum_reach_count += ba.reach_count;

    if (ba.num_blocks > max_block_count) max_block_count = ba.num_blocks;
    if (ba.num_blocks < min_block_count) min_block_count = ba.num_blocks;
    sum_block_count += ba.num_blocks;
  }

  std::cerr << "max in count:\t" << max_in_count << std::endl;
  std::cerr << "min in count:\t" << min_in_count << std::endl;
  std::cerr << "avg in count:\t" << ((double)sum_in_count) / (double)data.size()  << std::endl;

  std::cerr << "max reach count:\t" << max_reach_count << std::endl;
  std::cerr << "min reach count:\t" << min_reach_count << std::endl;
  std::cerr << "avg reach count:\t" << ((double)sum_reach_count) / (double)data.size()  << std::endl;

  std::cerr << "max block count:\t" << max_block_count << std::endl;
  std::cerr << "min block count:\t" << min_block_count << std::endl;
  std::cerr << "avg block count:\t" << ((double)sum_block_count) / (double)data.size()  << std::endl;
#endif
}

node general::add_node() {
  node id = data.size();
  data.push_back(bitset());
  return id;
}

bool general::precedes(node x, node y) {
  if (x == y) return true;

  // y should definitely be in data
  assert(data.size() > y);

  return data[y].get(x);
}

// void general::add_edges_from(node* from_nodes, int num_from, node to) {

//   assert(data[to].data == nullptr);
  
//   size_t max_size = 0;
//   for (auto i = 0; i < num_from; ++i) {
//     size_t s = data[from_nodes[i]].num_blocks;
//     if (s > max_size) max_size = s;
//   }
//   data[to].unsafe_resize(max_size);

//   for (auto i = 0; i < max_size; ++i) {
//     bitset::block_t b = 0;
//     for (auto j = 0; j < num_from; ++j) {
//       node from = from_nodes[j];
//       if (i < data[from].num_blocks) b |= data[from].data[i];
//     }
//     data[to].data[i] = b;
//   }

// }


// Assumes to has no out edges yet.
void general::add_edge(node from, node to) {
  if (from == to) return;

  if (data[to].data != nullptr) {
    data[to].resize(data[from].num_blocks);

    for (auto i = 0; i < data[from].num_blocks; ++i)
      data[to].data[i] |= data[from].data[i];

  } else { // fast case, no other incoming edges

    data[to] = bitset(data[from]);
  }
  data[to].set(from);

  // In the current implementation, this is slow b/c the set requires
  // a merge also, meaning we need two separate merges.
  // TODO: make R[i,i] always true in the bitsums DS
  // Doing so allows us to only do one merge.
  // data[to].set(from);
  // if (data[to].data == nullptr)
  //   data[to] = data[from];
  // else
    // data[to].merge(data[from]);

#ifdef STATS
  data[to].in_count++;
#endif


}
