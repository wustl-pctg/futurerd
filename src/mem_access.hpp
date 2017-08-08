#pragma once
#include <cstdint>

#include "spbag.hpp"

#define LOG_KEY_SIZE  4
#define LOG_TBL_SIZE 20

#define GRAIN_SIZE 4
#define LOG_GRAIN_SIZE 2
#define MAX_GRAIN_SIZE (1 << LOG_KEY_SIZE)
#define NUM_SLOTS (MAX_GRAIN_SIZE / GRAIN_SIZE)
 
// a mask that keeps all the bits set except for the least significant bits
// that represent the max grain size
#define MAX_GRAIN_MASK (~(uint64_t)(MAX_GRAIN_SIZE-1))

// If the value is already divisible by MAX_GRAIN_SIZE, return the value; 
// otherwise return the previous / next value divisible by MAX_GRAIN_SIZE.
#define ALIGN_BY_PREV_MAX_GRAIN_SIZE(addr) ((uint64_t) (addr & MAX_GRAIN_MASK))
#define ALIGN_BY_NEXT_MAX_GRAIN_SIZE(addr)                  \
  ((uint64_t) ((addr+(MAX_GRAIN_SIZE-1)) & MAX_GRAIN_MASK))

// compute (addr % 16) / GRAIN_SIZE 
#define ADDR_TO_MEM_INDEX(addr)                                       \
  (((uint64_t)addr & (uint64_t)(MAX_GRAIN_SIZE-1)) >> LOG_GRAIN_SIZE)

// compute size / GRAIN_SIZE 
#define SIZE_TO_NUM_GRAINS(size) (size >> LOG_GRAIN_SIZE) 

// struct access_info {
//   spbag *last_reader;
//   spbag *last_writer;
// };

class MemAccess_t {
private:
  spbag *m_access; /// bag that made the last access
  uint64_t m_rip; /// instruction address of this access

public:
  MemAccess_t(spbag* access, uint64_t rip)
    : m_access(access), m_rip(rip) {}

  bool races_with(MemAccess_t *other);

  // used to be update_acc_info in old code
  MemAccess_t& operator=(const MemAccess_t &other);
}; // class MemAccess_t

class MemAccessList_t {
private:
  uint64_t m_start_addr; /// smallest addr this list represents
  MemAccess_t *m_readers[NUM_SLOTS];
  MemAccess_t *m_writers[NUM_SLOTS];

  // TODO: take in a MemAccess_t instead of all this?
  bool check_read(uint64_t inst_addr, uint64_t mem_addr,
                  size_t mem_size, spbag *strand);
  bool check_write(uint64_t inst_addr, uint64_t mem_addr,
                  size_t mem_size, spbag *strand);

public:
  MemAccessList_t(uint64_t addr, spbag *strand,
                  bool is_read, // not sure why we need this in the ctor...
                  uint64_t rip, size_t mem_size); 
  ~MemAccessList_t();

  // necessary?
  __attribute__((always_inline)) bool
  check_access(bool is_read,
               uint64_t inst_addr, uint64_t mem_addr,
               size_t mem_size, spbag *strand) {
    return (is_read) ?
      check_read(inst_addr, mem_addr, mem_size, strand)
      :
      check_write(inst_addr, mem_addr, mem_size, strand);
  }

  void check_invariants(uint64_t current_func_id);
  
}; // class MemAccessList_t
