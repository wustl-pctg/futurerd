#include "mem_access.hpp"
#include <cassert>

bool
MemAccessList_t::check_read(uint64_t inst_addr, uint64_t mem_addr,
                            size_t mem_size, spbag *strand) {
  // Really this should call out to some kind of reachability data structure
  // spbag *last_writer = ???
  // return !last_writer->precedes_now();
  return false;
}

bool
MemAccessList_t::check_write(uint64_t inst_addr, uint64_t mem_addr,
                            size_t mem_size, spbag *strand) {
  return false;
}


MemAccessList_t::MemAccessList_t(uint64_t addr, spbag *strand,
                                 bool is_read, // why in the ctor
                                 uint64_t rip, size_t mem_size)
  : m_start_addr(ALIGN_BY_PREV_MAX_GRAIN_SIZE(addr)) {

  for (int i = 0; i < NUM_SLOTS; ++i) {
    m_readers[i] = m_writers[i] = nullptr;
  }

  const int start = ADDR_TO_MEM_INDEX(addr);
  const int grains = SIZE_TO_NUM_GRAINS(mem_size);
  assert(start >= 0 && start < NUM_SLOTS && (start + grains) <= NUM_SLOTS);

  if(is_read) {
    for(int i = start; i < (start + grains); ++i) {
      m_readers[i] = new MemAccess_t(strand, addr);
    }
  } else {
    for(int i = start; i < (start + grains); ++i) {
      m_writers[i] = new MemAccess_t(strand, addr);
    }
  }

}

MemAccessList_t::~MemAccessList_t() {
  for (auto slot : m_readers) {
    if (slot) {
      delete slot;
      slot = nullptr;
    }
  }

  for (auto slot : m_writers) {
    if (slot) {
      delete slot;
      slot = nullptr;
    }
  }
}
