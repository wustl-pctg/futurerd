// Conceptually, this is part of the race detector class. But these
// need to be plain C functions as defined by cilktool and Thread
// Sanitizer. Non-user entry points into the library, i.e. from the
// runtime and inserted by the compiler.
#include <dlfcn.h> // for dlsym

// This is a general cilk_tool "driver" class, meaning that it
// implements all the cilk_tool functions. It will enable/disable
// checking and instrumentation as needed, keep a valid shadow stack,
// and call tool::handle_<blah> functions
#include "rd.hpp"
typedef race_detector rd; // convenience

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

extern "C" {

// Public C API
void futurerd_set_policy(rd_policy p) { rd::set_policy(p); }
size_t futurerd_num_races() { return rd::num_races(); }

// The user uses these to wrap any code with benign races. We use them
// to protect accesses to the Cilk stack frame.
void futurerd_enable_checking() { rd::enable_checking(); }
void futurerd_disable_checking() { rd::disable_checking(); }
void futurerd_should_check() { rd::should_check(); }

// This is slightly dangerous since the initialization of the loop
// variable will not be changed. But this should not be a problem b/c
// the compiler already makes sure that the loop variable must be
// declared in the loop; it cannot be a shared local variable.
void cilk_for_begin() { rd::disable_checking(); }
void cilk_for_end() { rd::enable_checking(); }
void cilk_for_iteration_begin() { rd::enable_checking(); }

// We need to clear the stack immediately, since local variables are
// shared amonst logically-parallel iterations
void cilk_for_iteration_end() {
  rd::disable_checking();
  // __cilk_for_helper is calling this, so that helper's base pointer
  // is the stack_high_watermark to clear (stack grows downward)
  uint64_t hi = (uint64_t)__builtin_frame_address(1);
  uint64_t lo = (uint64_t)__builtin_frame_address(0);
  
  assert(lo && lo != (uint64_t)-1);
  assert(hi && lo <= hi);
  rd::g_smem.clear(lo, hi);
}

void cilk_enter_begin() {
  sframe_data *p = rd::t_sstack.head();
  rd::g_reach.begin_strand(rd::t_sstack.push_spawner(), p);
  rd::disable_checking();
}

// We know we're spawning here, but we can't call the appropriate
// functions yet because the arguments haven't been evaluated
// yet. That's why we do things in detach instead.
void cilk_enter_helper_begin(__cilkrts_stack_frame* sf,
                             void* this_fn, void* rip)
{ rd::disable_checking(); }

void cilk_enter_end(__cilkrts_stack_frame *sf, void *rsp)
{ rd::enable_checking(); }

void cilk_detach_begin(__cilkrts_stack_frame *parent_sf) {
  rd::g_reach.at_spawn(rd::t_sstack.head(), rd::t_sstack.do_spawn());
  //rd::g_reach.at_spawn(rd::t_sstack.push_helper());
  rd::disable_checking();
}

void cilk_detach_end() { rd::enable_checking(); }

// Real futures will need helper frames/functions.
void cilk_future_create() {
  assert(!rd::t_sstack.empty());

  // @TODO{Do we actually need a frame here?}
  // ANGE: Yes we do because we need a separate Sbags for the future frame and
  // its parent function that created the future.
  rd::g_reach.at_future_create(rd::t_sstack.push());
}

void cilk_future_get_begin(sfut_data *fut) { rd::disable_checking(); }
void cilk_future_get_end(sfut_data *fut) {
  rd::g_reach.at_future_get(rd::t_sstack.head(), fut);
  rd::enable_checking();
}

void cilk_sync_begin() { rd::disable_checking(); }

void cilk_sync_end(__cilkrts_stack_frame *sf) {
  // At the end of every Cilk function there is an implicit sync, even
  // if we're already synced...
  if (!rd::t_sstack.do_sync())
    rd::g_reach.at_sync(rd::t_sstack.head());
  rd::enable_checking();
}

void continuation() {
  rd::t_clear_stack = true;
  rd::t_sstack.pop(); // pop helper frame
}

// We don't support separate "put" operations right now. We assume the
// finish() call also put's the result.
void cilk_future_put_begin(sfut_data *fut) { rd::disable_checking(); }
void cilk_future_put_end(sfut_data *fut) { rd::enable_checking(); }

// XXX: should be different if using put/get futures!
void cilk_future_finish_begin(sfut_data *fut) { rd::disable_checking(); }
void cilk_future_finish_end(sfut_data *fut) {
  //assert(t_sstack.in_future_helper());
  sframe_data *f = rd::t_sstack.head();
  sframe_data *p = rd::t_sstack.parent();
  rd::g_reach.at_future_finish(f, p, fut);

  // When using fake futures, no cilk_leave_frame is called. When
  // using real futures we'll need to be fancier, since a worker may
  // steal the continuation.
  continuation();
  rd::enable_checking();
}

void cilk_leave_begin(__cilkrts_stack_frame* sf) {
  sframe_data *f = rd::t_sstack.head();

  // Currently I create a frame at start, so we should always have a parent.
  sframe_data *p = rd::t_sstack.parent();

  // returning from a spawn
  if (IS_HELPER(sf)) {
    rd::g_reach.at_spawn_continuation(f,p);

    // This is the point where we need to set flag to clear accesses to stack 
    // out of the shadow memory.  The spawn helper of this leave_begin
    // is about to return, and we want to clear stack accesses below 
    // (and including) spawn helper's stack frame.  Set the flag here
    // and the stack will be cleared in tsan_func_exit.
    continuation();
  }  else { // leaving a spawning frame
    rd::t_sstack.pop();
  }

  rd::disable_checking();
}

void cilk_leave_end() {
  // If rd::t_sstack.size() == 1, we just popped (in cilk_leave_begin())
  // the second-to-last frame. We're leaving a parallel section, so we don't
  // need to do any race detection.
  if (rd::t_sstack.size() > 1) rd::enable_checking();
}

// tsan functions

//void __tsan_destroy() {}
void __tsan_init() {
  static bool init = false;
  assert(init == false);
  init = true;

  //std::atexit(__tsan_destroy);

  // Originally this was called, but why?
  //rd::enable_checking();
}

static inline
void tsan_access(bool is_read, void *addr, size_t mem_size, void *rip) {
  if (!rd::should_check()) return;
  
  assert(mem_size <= 16);
  rd::check_access(is_read, rip, addr, mem_size);
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
  if(rd::t_stack_low_watermark > res)
    rd::t_stack_low_watermark = res;

  if (rd::t_clear_stack) {
    // the spawn helper that's exiting is calling tsan_func_exit, 
    // so the spawn helper's base pointer is the stack_high_watermark
    // to clear (stack grows downward)
    uint64_t stack_high_watermark = (uint64_t)__builtin_frame_address(1);

    assert( rd::t_stack_low_watermark != ((uint64_t)-1) );
    assert( rd::t_stack_low_watermark <= stack_high_watermark );
    rd::g_smem.clear(rd::t_stack_low_watermark, stack_high_watermark);
    // now the high watermark becomes the low watermark
    rd::t_stack_low_watermark = stack_high_watermark;
    rd::t_clear_stack = false;
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
  
  if (!rd::should_check())
    return real_malloc(s);

  // make it 8-byte aligned; easier to erase from shadow mem
  uint64_t new_size = ALIGN_BY_NEXT_MAX_GRAIN_SIZE(s);
  void *r = real_malloc(new_size);
  rd::g_smem.clear((uint64_t)r, (uint64_t)r + new_size);
  return r;
}


} // extern "C"
