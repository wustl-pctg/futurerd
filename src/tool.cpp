#include "tsan.hpp"
#include "debug.hpp"
#include "unionfind.hpp"

#include <internal/abi.h>

#include <cstdio>
#include <cassert>


#define TOOL_ASSERT(x) assert(x) /// @TODO{refactor asserts}

extern "C" {

void cilk_tool_init(void) { }
void cilk_tool_destroy(void) { }

void cilk_enter_begin() {
  DBG_TRACE();
  disable_checking();
}
void cilk_enter_helper_begin(__cilkrts_stack_frame* sf,
                             void* this_fn, void* rip) {
  DBG_TRACE();
  disable_checking(); 
}

void cilk_enter_end(__cilkrts_stack_frame *sf, void *rsp) {
  DBG_TRACE();
  enable_checking();
}

void cilk_sync_begin(__cilkrts_stack_frame* sf) {
  DBG_TRACE();
  disable_checking();
}
void cilk_sync_end() {
  DBG_TRACE();
  enable_checking();
}
void cilk_resume(__cilkrts_stack_frame* sf) {
  DBG_TRACE();
  enable_checking();
}

void cilk_spawn_prepare() {
  DBG_TRACE();
  disable_checking();
}
void cilk_spawn_or_continue(int in_continuation) {
  DBG_TRACE();
  enable_checking();
}
//void cilk_continue(__cilkrts_stack_frame* sf, char* new_sp) { }

void cilk_detach_begin(__cilkrts_stack_frame* sf,
                       void* this_fn, void* rip) {

  DBG_TRACE();
  disable_checking();
}
void cilk_detach_end() {
  DBG_TRACE();
  enable_checking();
}

void cilk_leave_begin(__cilkrts_stack_frame* sf) {
  DBG_TRACE();
  disable_checking();
}
void cilk_leave_end() {
  DBG_TRACE();
  enable_checking();
}

void cilk_steal_success(__cilkrts_worker *w, __cilkrts_worker *victim,
			__cilkrts_stack_frame *sf) {
  TOOL_ASSERT(t_checking_disabled > 0);
  // We don't need to enable checking here b/c this worker will
  // immediately execute cilk_spawn_or_continue
}

} // extern "C"
