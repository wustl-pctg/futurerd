#include "bitset.hpp"
#include <cstdint> // uint64_t
#include <cstdlib> // malloc

// Getting or setting a particular bit in a block
#define MASK(index) (1 << ((index) % sizeof(block_t)))

__attribute__ ((const))
static inline uint64_t p2(uint64_t x)
{
  return uint64_t(1) << (64 - __builtin_clzl(x - 1));
}

bitset::bitset(const bitset& that) {
  num_blocks = 0; // ensure resize really happens
  unsafe_resize(that.num_blocks);
  std::memcpy(data, that.data, num_blocks * sizeof(block_t));
}

void bitset::unsafe_resize(size_t new_size) {
  block_t* new_data = (block_t*) malloc(sizeof(block_t) * new_size);
  assert(new_data);
  std::memcpy(new_data, data, sizeof(block_t) * num_blocks);
  free(data);
  data = new_data;
  num_blocks = new_size;
}
    
void bitset::resize(size_t new_size) {
  if (new_size <= num_blocks) return;
  size_t old_size = num_blocks;
  unsafe_resize(new_size);
  std::memset(data + old_size, 0,
              (new_size - old_size) * sizeof(block_t));
}

void bitset::set(size_t y) {
  uint64_t block = y / BITS_PER_BLOCK;
  if (block >= num_blocks) {
    size_t new_size = p2(block+1);
    assert(block < new_size);
    resize(new_size);
  }
  assert(block < num_blocks);

  // Debugging
  block_t mask = MASK(y);
#ifdef STATS
  if (data[block] & mask) reach_count++;
#endif
  data[block] |= mask;
}

bool bitset::get(size_t y) const {
  size_t block = y / BITS_PER_BLOCK;
  if (block >= num_blocks) return false;
  return data[block] & MASK(y);
}
