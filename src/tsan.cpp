#include <cstdlib> // atexit
#include <cassert>

#include "tsan.hpp"
#include "debug.hpp"

extern "C" {

static bool g_instr_enabled = false;
__thread int t_checking_disabled = 1;

extern void cilk_tool_init(void);
extern void cilk_tool_destroy(void);

void __tsan_destroy() {
  cilk_tool_destroy();
  g_instr_enabled = false;
  assert(t_checking_disabled == 0);
  disable_checking();
}

void __tsan_init() {

  static bool init = false;

  /// @TODO{For some reason __tsan_init gets called twice...?}
  // if (init) return;
  assert(init == false);
  init = true;

  atexit(__tsan_destroy);

  cilk_tool_init();
  g_instr_enabled = true; 
  enable_checking();
}

[[gnu::always_inline]] static
bool should_check() { return(g_instr_enabled && t_checking_disabled == 0); }


static inline
void tsan_read(void *addr, size_t mem_size, void *rip) {
  if (should_check()) DBG_TRACE("checking enabled\n");
  else DBG_TRACE("checking disabled\n");
}

static inline
void tsan_write(void *addr, size_t mem_size, void *rip) {
  if (should_check()) DBG_TRACE("checking enabled\n");
  else DBG_TRACE("checking disabled\n");
}

void __tsan_read1(void *addr) { tsan_read(addr, 1, __builtin_return_address(0)); }
void __tsan_read2(void *addr) { tsan_read(addr, 2, __builtin_return_address(0)); }
void __tsan_read4(void *addr) { tsan_read(addr, 4, __builtin_return_address(0)); }
void __tsan_read8(void *addr) { tsan_read(addr, 8, __builtin_return_address(0)); }
void __tsan_read16(void *addr) { tsan_read(addr, 16, __builtin_return_address(0)); }
void __tsan_write1(void *addr) { tsan_write(addr, 1, __builtin_return_address(0)); }
void __tsan_write2(void *addr) { tsan_write(addr, 2, __builtin_return_address(0)); }
void __tsan_write4(void *addr) { tsan_write(addr, 4, __builtin_return_address(0)); }
void __tsan_write8(void *addr) { tsan_write(addr, 8, __builtin_return_address(0)); }
void __tsan_write16(void *addr) { tsan_write(addr, 16, __builtin_return_address(0)); }
void __tsan_func_entry(void *pc){ }
void __tsan_func_exit() { }
void __tsan_vptr_read(void **vptr_p) {}
void __tsan_vptr_update(void **vptr_p, void *new_val) {}

} // extern "C"
