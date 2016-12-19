void __tsan_destroy() { g_instr_enabled = false; print_stats(); }
void __tsan_init() {

  // For some reason __tsan_init gets called twice...
  static int init = 0;
  if (init) return;
  init = 1;

  atexit(__tsan_destroy);
  g_instr_enabled = true; 
  enable_checking();
}

static inline void
tsan_read(void *addr, size_t mem_size, void *rip) { 
  if (should_check()) INCR(num_reads);
}

static inline void
tsan_write(void *addr, size_t mem_size, void *rip) {
  if (should_check()) INCR(num_writes);
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
