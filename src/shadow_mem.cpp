#include "shadow_mem.hpp"
#include <cassert>

using addr_t = shadow_mem::addr_t;

shadow_mem::addr_info_t* shadow_mem::find(addr_t key) {
  auto it = m_shadow_map.find(key);
  return (it != m_shadow_map.end()) ? &it->second : nullptr;
}

shadow_mem::addr_info_t* shadow_mem::insert(bool is_read, addr_t addr,
                                            rd_info_t strand, addr_t rip) {
  addr_info_t *old = find(addr);
  addr_info_t *slot = old;
  if (slot == nullptr) {
    auto option = m_shadow_map.insert(std::make_pair(addr, addr_info_t()));
    assert(option.second); // indicates success of insert operation
    slot = &option.first->second;
  }
  assert(slot);
  access_t *access = (is_read) ? &slot->last_reader : &slot->last_writer;
  *access = access_t(strand, rip);
  return old;
}

void shadow_mem::update(addr_info_t *slot, bool is_read,
            addr_t addr, rd_info_t strand, addr_t rip) {
  assert(slot != nullptr);
  access_t *access = (is_read) ? &slot->last_reader : &slot->last_writer;
  *access = access_t(strand, rip);
}

void shadow_mem::clear(addr_t start, addr_t end) {
  assert(ALIGN_BY_NEXT_MAX_GRAIN_SIZE(end) == end); 
  assert(start < end);

  // Not true for large malloc'ed blocks...
  //assert(end-start < 4096);

  while(start != end) {
    m_shadow_map.erase(start);
    start += GRAIN_SIZE;
  }
}
