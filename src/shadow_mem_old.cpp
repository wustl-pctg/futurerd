#include "shadow_mem.hpp"
#include <cassert>

ShadowMem::ShadowMem()
  : m_dir(new struct shadow_tbl *[1<<(48 - LOG_TBL_SIZE - LOG_KEY_SIZE)]) {}

ShadowMem::~ShadowMem() {}


/* XXX: we don't synchronize on erase --- this is called whenever  
 * we malloc a new block of memory (so shadow memory associated with the 
 * allocation is cleared), or when we return from spawned function 
 * (the cactus stack corresponding to the spawned function is cleared).
 * If another thread is accessing this memory while we clear it, the program
 * is accessing freed pointer or deallocated stack, which we assume does 
 * not occur.
 */
void ShadowMem::erase(uint64_t key) {
  MemAccessList_t **slot = find_slot(key, false);
  if (slot != NULL)
    *slot = NULL;
}


MemAccessList_t** ShadowMem::find_slot(uint64_t key, bool alloc) {
  /* I think this volatile is necessary and sufficient ... */
  shadow_tbl *volatile *dest = &(m_dir[key>>LOG_TBL_SIZE]);
  shadow_tbl *tbl = *dest;

  if (!alloc && !tbl) {
    return NULL;
  } else if (tbl == NULL) {
    struct shadow_tbl *new_tbl = new struct shadow_tbl();
    do {
      tbl = __sync_val_compare_and_swap(dest, tbl, new_tbl);
    } while(tbl == NULL);
    assert(tbl != NULL);

    if(tbl != new_tbl) { // someone got to the allocation first
      delete new_tbl; 
    }
  }
  MemAccessList_t** slot =  &tbl->shadow_entries[key&((1<<LOG_TBL_SIZE) - 1)];
  return slot;
}


void ShadowMem::clear(size_t start, size_t end) {
  assert(ALIGN_BY_NEXT_MAX_GRAIN_SIZE(end) == end); 
  assert(start < end);
  assert(end-start < 4096);

  while(start != end) {
    erase( ADDR_TO_KEY(start) );
    start += MAX_GRAIN_SIZE;
  }
}

// called by record_memory_read/write, with the access broken down 
// into 8-byte aligned memory accesses
bool ShadowMem::racy_access(bool is_read, uint64_t inst_addr,
                       uint64_t addr, uint32_t mem_size,
                       spbag *strand)
{
  MemAccessList_t *val = find( ADDR_TO_KEY(addr) );
  MemAccessList_t *list = NULL;

  if( val == NULL ) {
    // not in shadow memory; create a new MemAccessList_t and insert
    list = new MemAccessList_t(addr, strand, is_read, inst_addr, mem_size);
    val = insert(ADDR_TO_KEY(addr), list);
    return false;
  }
  
  // check for race and possibly update the existing MemAccessList_t 
  return val->check_access(is_read, inst_addr, addr, mem_size, strand);
}
