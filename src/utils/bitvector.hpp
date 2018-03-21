#include <cstring> // memcpy, memset
#include <cassert>

//#define STATS 1

struct bitset {

  using block_t = unsigned int;
  static constexpr size_t BITS_PER_BLOCK = sizeof(block_t) * 8;
  static constexpr size_t DEFAULT_SIZE = 0;
    
  size_t num_blocks = 0;
  block_t* data = nullptr;
  // node in = 0;

  // Debugging
#ifdef STATS
  size_t in_count = 0;
  size_t reach_count = 0;
#endif
    
  bitset() { resize(DEFAULT_SIZE); }
  bitset(const bitset& that);
  bitset(bitset&& that) = default;
  bitset& operator=(const bitset& src) = default;

  // Doesn't initialize newly allocated blocks
  void unsafe_resize(size_t new_size);

  void resize(size_t new_size);

  // Normal operations
  void set(size_t y);
  bool get(size_t y) const;

};
