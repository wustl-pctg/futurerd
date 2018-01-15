#pragma once
#include <cstdio>
#include <unordered_map>

#include "spbag.hpp"
#include "reach.hpp"

// Some of the driver code uses this stuff...
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

//template<class mem_access_data>
class shadow_mem {
public:
  using addr_t = uint64_t;
  using rd_info_t = smem_data*;
  struct access_t {
    rd_info_t access;
    addr_t rip;
    access_t() : access(nullptr), rip(0) {}
    access_t(rd_info_t a, addr_t r)
      : access(a), rip(r) {}
  };

  struct addr_info_t {
    access_t last_reader;
    access_t last_writer;
    addr_info_t() {}
  };

  shadow_mem() { m_shadow_map.reserve(32); }
  
  addr_info_t* find(addr_t addr);

  // return the old value
  addr_info_t* insert(bool is_read, addr_t addr, rd_info_t strand, addr_t rip);
  void update(addr_info_t *slot, bool is_read,
              addr_t addr, rd_info_t strand, addr_t rip);
  void clear(addr_t start, addr_t end);

private:
  //std::map<addr_t, addr_info_t> m_shadow_map;
  std::unordered_map<addr_t, addr_info_t> m_shadow_map;
};
