// see the cilksan/src/driver.cpp file for more

// This is a general cilk_tool "driver" class, meaning that it
// implements all the cilk_tool functions. It will enable/disable
// checking and instrumentation as needed, keep a valid shadow stack,
// and call tool::handle_<blah> functions
#include "futurerd.hpp"
#include "debug.hpp"

#define USE_CILK_API
#include <internal/abi.h>
#include <cilk/cilk_api.h>

#include <cassert>
#include <cstdlib> // atexit

extern "C" {

static bool TOOL_INITIALIZED = false;

// originally this was set to false, but why?
static bool t_checking_disabled = true;
//static bool check_enable_instrumentation;
//static bool instrumentation;

//[[gnu::always_inline]]
// __attribute__((always_inline))
// static void enable_instrumentation() { instrumentation = true; }

// __attribute__((always_inline))
// static void disable_instrumentation() { instrumentation = false; }

__attribute__((always_inline))
static void enable_checking() {
  //assert(t_checking_disabled);
  t_checking_disabled = false;
}

__attribute__((always_inline))
static void disable_checking() {
  //assert(!t_checking_disabled);
  t_checking_disabled = true;
}


void cilk_spawn_prepare() { DBG_TRACE(); disable_checking(); }

void cilk_spawn_or_continue(int in_continuation) { DBG_TRACE(); enable_checking(); }

void cilk_enter_begin() { DBG_TRACE();
  static bool should_init = true;

  /// @REFACTOR{No reason to init in cilk_enter_begin since we can use __tsan_init}
  if(__builtin_expect(should_init, false)) {
    futurerd::init();
    should_init = false;
    TOOL_INITIALIZED = true;
  } else {
    disable_checking();
    assert(TOOL_INITIALIZED);
    futurerd::at_cilk_function();
  }
}

void cilk_enter_end(__cilkrts_stack_frame *sf, void *rsp) { DBG_TRACE();

  // What is this for?
  // if(__builtin_expect(check_enable_instrumentation, 0)) {
  //   check_enable_instrumentation = false;
  //   enable_instrumentation();
  // }
  
  // My code had this??
  // if (t_sstack.empty()) {
  //   init_strand();
  // // instrumentation stuff
    // stack_low_watermark = (uint64_t)(-1);
  // }

  enable_checking();
}

// This signature?
// void cilk_enter_helper_begin(__cilkrts_stack_frame* sf,
//                              void* this_fn, void* rip)
void cilk_enter_helper_begin() { DBG_TRACE();
  disable_checking();
  futurerd::at_spawn();
}

// void cilk_detach_begin(__cilkrts_stack_frame* sf,
//                        void* this_fn, void* rip) {
void cilk_detach_begin(__cilkrts_stack_frame *parent_sf) { DBG_TRACE(); disable_checking(); }

void cilk_detach_end() { DBG_TRACE(); enable_checking(); }

void cilk_sync_begin() { DBG_TRACE();
  disable_checking(); 
  assert(TOOL_INITIALIZED);
}

void cilk_sync_end() { DBG_TRACE();
  futurerd::at_sync();
  assert(TOOL_INITIALIZED);
  enable_checking();
}

void cilk_leave_begin(void *p) { DBG_TRACE();
  disable_checking();
  futurerd::at_leave_begin();
}

void cilk_leave_end() { DBG_TRACE();
  bool last_frame = futurerd::at_leave_end();
  // enable_checking();
  // if(last_frame == true) { // last frame
  //   disable_instrumentation();
  //   check_enable_instrumentation = true;
  // }
  if (!last_frame)
    enable_checking();
}

// tsan functions

void __tsan_destroy() {
  //g_instr_enabled = false;

  // we're already assured to disable in cilk_leave_end
  // assert(t_checking_disabled == true);
  // disable_checking();
}

void __tsan_init() {

  static bool init = false;

  /// @TODO{For some reason __tsan_init gets called twice...?}
  // if (init) return;
  assert(init == false);
  init = true;

  std::atexit(__tsan_destroy);

  //g_instr_enabled = true;

  // Originally this was called, but why?
  //enable_checking();
}

[[gnu::always_inline]] static
bool should_check() { return !t_checking_disabled; }
//{ return(g_instr_enabled && t_checking_disabled == 0); }


static inline
void tsan_read(void *addr, size_t mem_size, void *rip) {
  if (should_check()) 
    futurerd::check_read(addr);
}

static inline
void tsan_write(void *addr, size_t mem_size, void *rip) {
  if (should_check()) 
    futurerd::check_write(addr);
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
