// work in progress
// see the cilksan/src/driver.cpp file for more

// This is a general cilk_tool "driver" class, meaning that it
// implements all the cilk_tool functions. It will enable/disable
// checking and instrumentation as needed, keep a valid shadow stack,
// and call tool::handle_<blah> functions
#include "futurerd.hpp"

#define USE_CILK_API
#include <internal/abi.h>
#include <cilk/cilk_api.h>

#include <cassert>

extern "C" {

static bool TOOL_INITIALIZED = false;
static bool check_enable_instrumentation = true;

// When either is set to false, no errors are output
static bool instrumentation = false;
// needs to be reentrant due to reducer operations; 0 means checking
static bool t_checking_disabled = false;

__attribute__((always_inline))
static void enable_instrumentation() { instrumentation = true; }

__attribute__((always_inline))
static void disable_instrumentation() { instrumentation = false; }

__attribute__((always_inline))
static void enable_checking() { t_checking_disabled = false; }

__attribute__((always_inline))
static void disable_checking() { t_checking_disabled = true; }


void cilk_spawn_prepare() { disable_checking(); }

void cilk_spawn_or_continue(int in_continuation) { enable_checking(); }

void cilk_enter_begin() {
  static bool should_init = true;
  disable_checking();

  if(should_init) {
    futurerd::init();
    should_init = false;
  }
  assert(TOOL_INITIALIZED);
  futurerd::at_cilk_function();
}

void cilk_enter_end(__cilkrts_stack_frame *sf, void *rsp) {

  if(__builtin_expect(check_enable_instrumentation, 0)) {
    check_enable_instrumentation = false;
    enable_instrumentation();
  }
  // My code had this??
  // if (t_sstack.empty()) {
  //   init_strand();
  // // instrumentation stuff
    // stack_low_watermark = (uint64_t)(-1);
  // }

  enable_checking();
}

// my tool:
// enter_begin (push)
// detach_begin (push_helper)
// leave_stolen (pop)
// leave_end (pop)

// enter_begin (entry_stack)
// enter_helper_begin (entry_stack)
// enter_begin -> enter_cilk_function -> start_new_function (frame_stack)
// detach_end -> enter_spawn_child -> start_new_function (frame_stack)

// leave_end (entry_stack)
// leave_stolen_callback ? (entry_stack)
// leave_begin -> leave_cilk_function || return_from_spawn
// leave_cilk_function -> exit_function (frame_stack)
// return_from_spawn -> exit_function (frame_stack)


// This signature?
// void cilk_enter_helper_begin(__cilkrts_stack_frame* sf,
//                              void* this_fn, void* rip)
void cilk_enter_helper_begin() {
  disable_checking();
  futurerd::at_spawn();
}

// void cilk_detach_begin(__cilkrts_stack_frame* sf,
//                        void* this_fn, void* rip) {
void cilk_detach_begin(__cilkrts_stack_frame *parent_sf) { disable_checking(); }

void cilk_detach_end() { enable_checking(); }

void cilk_sync_begin() {
  disable_checking(); 
  assert(TOOL_INITIALIZED);
}

void cilk_sync_end() {
  futurerd::at_sync();
  assert(TOOL_INITIALIZED);
  enable_checking();
}

void cilk_leave_begin(void *p) { disable_checking(); }

void cilk_leave_end() {
  bool last_frame = futurerd::at_leave_end();
  enable_checking();
  if(last_frame == true) { // last frame
    disable_instrumentation();
    check_enable_instrumentation = true;
  }
}

} // extern "C"
