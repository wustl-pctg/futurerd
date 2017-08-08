#pragma once
#include <cstdio>

// macro for address manipulation for shadow mem
#define ADDR_TO_KEY(addr) ((uint64_t) ((uint64_t)addr >> LOG_KEY_SIZE))
#include "mem_access.hpp"


class ShadowMem {
private:
  //using shadow_tbl = MemAccessList_t* [1<<LOG_TBL_SIZE];
  struct shadow_tbl {
    MemAccessList_t *shadow_entries[1<<LOG_TBL_SIZE];
  };
  struct shadow_tbl **m_dir;

  MemAccessList_t** find_slot(uint64_t key, bool alloc);

public:
  ShadowMem();

  inline MemAccessList_t* find(uint64_t key) {
    MemAccessList_t **slot = find_slot(key, false);
    if (slot == NULL)
      return NULL;
    return *slot;
  }

  //  return the value at the memory location when insert occurs
  //  If the value returned != val, insertion failed because someone
  //  else got to the slot first.  
  inline MemAccessList_t * insert(uint64_t key, MemAccessList_t *val) {
    MemAccessList_t *volatile *slot = find_slot(key, true);
    MemAccessList_t *old = *slot;
    *slot = val;
    return old;
  }

  void clear(size_t start, size_t end);
  void erase(uint64_t key);
  bool racy_access(bool is_read, uint64_t inst_addr,
              uint64_t addr, uint32_t mem_size,
              spbag *strand);

  ~ShadowMem();

};
n
