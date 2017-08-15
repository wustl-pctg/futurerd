// Conceptually, this is part of the race detector class. But these
// need to be plain C functions as defined by cilktool and Thread
// Sanitizer. Non-user entry points into the library, i.e. from the
// runtime and inserted by the compiler.
#include <dlfcn.h> // for dlsym

// This is a general cilk_tool "driver" class, meaning that it
// implements all the cilk_tool functions. It will enable/disable
// checking and instrumentation as needed, keep a valid shadow stack,
// and call tool::handle_<blah> functions
//#include "futurerd.hpp"
#include "rd.hpp"

#include "shadow_mem.hpp"
#include "debug.hpp"

#define USE_CILK_API
#include <internal/abi.h> // for __cilkrts_stack_frame->flags (IS_HELPER)
#include <cilk/cilk_api.h>

#include <cassert>
#include <cstdlib> // atexit

// In lieu of having two types of shadow frames:
// Note: only works AFTER detach
#define IS_HELPER(sf) (sf->flags & CILK_FRAME_DETACHED)

// Choose your reachability data structure
//using rd_t = rd::structure_rd;
//structured_reach g_reach;
extern race_detector g_rd;

extern "C" {

// Public C API
void set_policy(rd::policy p) { g_rd.set_policy(p) }
size_t num_races() { return g_rd.num_races(); }

// The user uses these to wrap any code with benign races. We use them
// to protect accesses to the Cilk stack frame.
void enable_checking() { g_rd.enable_checking(); }
void disable_checking() { g_rd.disable_checking(); }
void should_check() { g_rd.should_check(); }

// XXX: Remove or use
void cilk_spawn_prepare() {}
void cilk_spawn_or_continue(int in_continuation) {}

void cilk_enter_begin() {
  // XXX: Make sure we never pop off the initial frame
  assert(!g_rd.t_sstack.empty());
  //g_rd.t_sstack.push_spawner();

  // rd::frame_data *parent = nullptr;

  // if (g_rd.t_sstack.size() == 1) { // first frame
  //   // We can't just do this in init since we may enter the "first"
  //   // frame multiple times (go in and out of Cilk).
  //   t_stack_low_watermark = (uint64_t)(-1);
  // } else {
  //   parent = g_rd.t_sstack.parent();
  // }
  
  // rd::frame_data *child = g_rd.t_sstack.head();
  // g_rd.at_cilk_function_start(child, parent);
  g_rd.disable_checking();

}

void cilk_enter_helper_begin(__cilkrts_stack_frame* sf,
                             void* this_fn, void* rip) {
  g_rd.disable_checking();
  //g_rd.at_spawn(g_rd.t_sstack.push_helper());
  g_rd.at_spawn(g_rd.t_sstack.push());
}

// Someday we may need a helper function for futures?
void cilk_future_create() {
  // Since we're faking futures, cilk_enter_{begin,end} may not get called
  // if (g_rd.t_sstack.size == 0
  //     || g_rd.t_sstack.in_helper()) {
  //   cilk_enter_begin();
  //   cilk_enter_end();
  // }
  assert(!g_rd.t_sstack.empty());
    
  g_rd.at_future_create(g_rd.t_sstack.push());
}

void cilk_future_get(sfut_data *fut) {
  g_rd.at_future_get(g_rd.t_sstack.head(), fut);
}

void cilk_enter_end(__cilkrts_stack_frame *sf, void *rsp) {
  g_rd.enable_checking();
}

void cilk_detach_begin(__cilkrts_stack_frame *parent_sf) {
  g_rd.disable_checking();
}

void cilk_detach_end() {

  g_rd.enable_checking();
}

void cilk_sync_begin() {
  g_rd.disable_checking(); 
}

void cilk_sync_end(__cilkrts_stack_frame *sf) {
  // XXX: Only call g_rd.at_sync() when we're really at a sync...
  g_rd.at_sync(g_rd.t_sstack.head());
  g_rd.enable_checking();
}

void continuation() {
  t_clear_stack = true;
  g_rd.t_sstack.pop();
}

// XXX: should be different if using put/get futures!
void cilk_future_finish(sfut_data *fut) {
  //assert(t_sstack.in_future_helper());
  rd::frame_data *f = g_rd.t_sstack.head();
  rd::frame_data *p = g_rd.t_sstack.parent();
  g_rd.at_future_finish(f, p, fut);

  // when using fake futures, no cilk_leave_frame is called.
  continuation();
}

void cilk_leave_begin(__cilkrts_stack_frame* sf) {
  rd::frame_data *f = g_rd.t_sstack.head();
  rd::frame_data *p = g_rd.t_sstack.parent();

  // returning from a spawn
  //if (g_rd.t_sstack.in_spawn_helper()) {
  if (IS_HELPER(sf)) {
    g_rd.at_spawn_continuation(f,p);

    // This is the point where we need to set flag to clear accesses to stack 
    // out of the shadow memory.  The spawn helper of this leave_begin
    // is about to return, and we want to clear stack accesses below 
    // (and including) spawn helper's stack frame.  Set the flag here
    // and the stack will be cleared in tsan_func_exit.
    //t_clear_stack = true;
    //g_rd.t_sstack.pop();
    continuation();
    
  } else { // ending cilk function
    //at_cilk_function_end(f,p);
  }
  
  //g_rd.t_sstack.pop();
  g_rd.disable_checking();
}

void cilk_leave_end() {
  // If g_rd.t_sstack.size() == 1, we just popped (in cilk_leave_begin())
  // the last frame. We're leaving a parallel section, so we don't
  // need to do any race detection.
  if (g_rd.t_sstack.size() > 1) g_rd.enable_checking();
}

// tsan functions

//void __tsan_destroy() {}
void __tsan_init() {
  static bool init = false;
  assert(init == false);
  init = true;

  //std::atexit(__tsan_destroy);

  // Originally this was called, but why?
  //g_rd.enable_checking();
}

static inline
void tsan_access(bool is_read, void *addr, size_t mem_size, void *rip) {
  if (!g_rd.should_check()) return;
  
  assert(mem_size <= 16);
  g_rd.check_access(is_read, rip, addr, mem_size);
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
  uint64_t res = (uint64_t) __builtin_frame_address(0);
  if(t_stack_low_watermark > res)
    t_stack_low_watermark = res;

  if (t_clear_stack) {
    // the spawn helper that's exiting is calling tsan_func_exit, 
    // so the spawn helper's base pointer is the stack_high_watermark
    // to clear (stack grows downward)
    uint64_t stack_high_watermark = (uint64_t)__builtin_frame_address(1);

    assert( t_stack_low_watermark != ((uint64_t)-1) );
    assert( t_stack_low_watermark <= stack_high_watermark );
    g_smem.clear(t_stack_low_watermark, stack_high_watermark);
    // now the high watermark becomes the low watermark
    t_stack_low_watermark = stack_high_watermark;
    t_clear_stack = false;
  }
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
  
  if (! (TOOL_INITIALIZED && g_rd.should_check()))
    return real_malloc(s);

  // make it 8-byte aligned; easier to erase from shadow mem
  uint64_t new_size = ALIGN_BY_NEXT_MAX_GRAIN_SIZE(s);
  void *r = real_malloc(new_size);
  g_shadow_mem.clear((uint64_t)r, (uint64_t)r + new_size);
  return r;
}


} // extern "C"
