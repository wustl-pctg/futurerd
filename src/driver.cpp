// see the cilksan/src/driver.cpp file for more
#include <dlfcn.h> // for dlsym

// This is a general cilk_tool "driver" class, meaning that it
// implements all the cilk_tool functions. It will enable/disable
// checking and instrumentation as needed, keep a valid shadow stack,
// and call tool::handle_<blah> functions
#include "futurerd.hpp"
#include "shadow_mem.hpp"
#include "debug.hpp"

#define USE_CILK_API
#include <internal/abi.h>
#include <cilk/cilk_api.h>

#include <cassert>
#include <cstdlib> // atexit

extern "C" {

static bool TOOL_INITIALIZED = false;

//static bool check_enable_instrumentation;
//static bool instrumentation;

//[[gnu::always_inline]]
// __attribute__((always_inline))
// static void enable_instrumentation() { instrumentation = true; }

// __attribute__((always_inline))
// static void disable_instrumentation() { instrumentation = false; }
static __thread uint64_t t_stack_low_watermark = (uint64_t)-1;

void cilk_spawn_prepare() { DBG_TRACE();
  futurerd::disable_checking();
}

void cilk_spawn_or_continue(int in_continuation) { DBG_TRACE();
  futurerd::enable_checking();
}

void cilk_enter_begin() { DBG_TRACE();
  //static bool should_init = true;

  /// @REFACTOR{No reason to init in cilk_enter_begin since we can use __tsan_init}
  // if(__builtin_expect(should_init, false)) {
  //   futurerd::init();
  //   should_init = false;
  //   TOOL_INITIALIZED = true;
  // } else {
  assert(TOOL_INITIALIZED);
  futurerd::disable_checking();
  bool first_frame = futurerd::at_cilk_function();

  // We can't just do this in init since we may enter the "first"
  // frame multiple times (go in and out of Cilk). OR we could do this
  // when we notice we leave the last frame...
  if (first_frame) {
    t_stack_low_watermark = (uint64_t)(-1);
  }
  // }
}

void cilk_enter_end(__cilkrts_stack_frame *sf, void *rsp) { DBG_TRACE();

  // What is this for?
  // if(__builtin_expect(check_enable_instrumentation, 0)) {
  //   check_enable_instrumentation = false;
  //   enable_instrumentation();
  // }
  
  // My code had this??
  // if (t_sstack.size() <= 1) {
  //   assert(t_sstack.size() == 1);
  //   //init_strand();
  //   stack_low_watermark = (uint64_t)(-1);
  // }

  futurerd::enable_checking();
}

// This signature?
// void cilk_enter_helper_begin(__cilkrts_stack_frame* sf,
//                              void* this_fn, void* rip)
void cilk_enter_helper_begin() { DBG_TRACE();
  futurerd::disable_checking();
  futurerd::at_spawn();
}

// void cilk_detach_begin(__cilkrts_stack_frame* sf,
//                        void* this_fn, void* rip) {
void cilk_detach_begin(__cilkrts_stack_frame *parent_sf) { DBG_TRACE(); futurerd::disable_checking(); }

void cilk_detach_end() { DBG_TRACE(); futurerd::enable_checking(); }

void cilk_sync_begin() { DBG_TRACE();
  futurerd::disable_checking(); 
  assert(TOOL_INITIALIZED);
}

void cilk_sync_end(__cilkrts_stack_frame *sf) { DBG_TRACE();
  // XXX: Only call futurerd::at_sync() when we're really at a sync...
  futurerd::at_sync();
  assert(TOOL_INITIALIZED);
  futurerd::enable_checking();
}

void cilk_leave_begin(__cilkrts_stack_frame* sf) { DBG_TRACE();
  futurerd::disable_checking();
  futurerd::at_leave_begin();
}

void cilk_leave_end() { DBG_TRACE();
  bool last_frame = futurerd::at_leave_end();
  // futurerd::enable_checking();
  // if(last_frame == true) { // last frame
  //   disable_instrumentation();
  //   check_enable_instrumentation = true;
  // }
  if (!last_frame)
    futurerd::enable_checking();
}

// tsan functions

void __tsan_destroy() {
  //g_instr_enabled = false;

  // we're already assured to disable in cilk_leave_end
  // assert(t_checking_disabled == true);
  // futurerd::disable_checking();
}

void __tsan_init() {
  static bool init = false;

  /// @TODO{For some reason __tsan_init gets called twice...?}
  // if (init) return;
  assert(init == false);
  init = true;

  futurerd::init();
  TOOL_INITIALIZED = true;

  std::atexit(__tsan_destroy);

  //g_instr_enabled = true;

  // Originally this was called, but why?
  //futurerd::enable_checking();
}

static inline
void tsan_access(bool is_read, void *addr, size_t mem_size, void *rip) {
  assert(TOOL_INITIALIZED);
  if (!futurerd::should_check()) return;
  
  futurerd::disable_checking();
  assert(mem_size <= 16);
  futurerd::check_access(is_read, rip, addr, mem_size);
  futurerd::enable_checking();
}

void __tsan_read1(void *addr) { tsan_access(true, addr, 1, __builtin_return_address(0)); }
void __tsan_read2(void *addr) { tsan_access(true, addr, 2, __builtin_return_address(0)); }
void __tsan_read4(void *addr) { tsan_access(true, addr, 4, __builtin_return_address(0)); }
void __tsan_read8(void *addr) { tsan_access(true, addr, 8, __builtin_return_address(0)); }
void __tsan_read16(void *addr) { tsan_access(true, addr, 16, __builtin_return_address(0)); }
void __tsan_write1(void *addr) { tsan_access(false, addr, 1, __builtin_return_address(0)); }
void __tsan_write2(void *addr) { tsan_access(false, addr, 2, __builtin_return_address(0)); }
void __tsan_write4(void *addr) { tsan_access(false, addr, 4, __builtin_return_address(0)); }
void __tsan_write8(void *addr) { tsan_access(false, addr, 8, __builtin_return_address(0)); }
void __tsan_write16(void *addr) { tsan_access(false, addr, 16, __builtin_return_address(0)); }
void __tsan_func_entry(void *pc){ }
void __tsan_vptr_read(void **vptr_p) {}
void __tsan_vptr_update(void **vptr_p, void *new_val) {}

/* We would like to clear the shadow memory correponding to the cactus
 * stack whenever we leave a Cilk function.  Unfortunately, variables are 
 * still being read after cilk_leave_frame_begin (such as return value 
 * stored on stack, so we really have to clean the shadow mem for stack 
 * at the vary last moment, right before we return, which seems to be 
 * where tsan_func_exit is called.  So instead, we set a thread-local 
 * flag to tell this worker to clean the shadow mem correponding to its 
 * stack at cilk_leave_begin, but check the flag and actually do the cleaning
 * in __tsan_func_exit.
 */
void __tsan_func_exit() {
  // TODO: clear shadow stack at the right places
  // TOOD: add high/low watermark member vars to shadow_stack class
  // TODO: update high/low watermark at the correct places
  //futurerd::g_shadow_mem.clear((uint64_t)r, (uint64_t)r + new_size);

  //futurerd::disable_checking();
  uint64_t res = (uint64_t) __builtin_frame_address(0);
  if(t_stack_low_watermark > res)
    t_stack_low_watermark = res;

  if (futurerd::t_clear_stack) {
    // the spawn helper that's exiting is calling tsan_func_exit, 
    // so the spawn helper's base pointer is the stack_high_watermark
    // to clear (stack grows downward)
    uint64_t stack_high_watermark = (uint64_t)__builtin_frame_address(1);

    assert( t_stack_low_watermark != ((uint64_t)-1) );
    assert( t_stack_low_watermark <= stack_high_watermark );
    futurerd::g_shadow_mem.clear(t_stack_low_watermark, stack_high_watermark);
    // now the high watermark becomes the low watermark
    t_stack_low_watermark = stack_high_watermark;
    futurerd::t_clear_stack = false;
  }
  //futurerd::enable_checking();
}

// Let's hope we don't need to wrap both calloc and realloc...
typedef void*(*malloc_t)(size_t);
void* malloc(size_t s) {

  static malloc_t real_malloc = NULL;

  if (real_malloc == NULL) {
    real_malloc = (malloc_t)dlsym(RTLD_NEXT, "malloc");
    char *error = dlerror();
    if (error != NULL) {
      fputs(error, stderr);
      fflush(stderr);
      abort();
    }
  }
  
  if (! (TOOL_INITIALIZED && futurerd::should_check()))
    return real_malloc(s);

  // make it 8-byte aligned; easier to erase from shadow mem
  uint64_t new_size = ALIGN_BY_NEXT_MAX_GRAIN_SIZE(s);
  void *r = real_malloc(new_size);
  futurerd::g_shadow_mem.clear((uint64_t)r, (uint64_t)r + new_size);
  return r;
}


} // extern "C"
