#include "tsan.hpp"
#include "debug.hpp"
#include "utils/union_find.hpp"
#include "utils/stack.hpp"
#include "sp_reach.hpp"

#define USE_CILK_API
#include <internal/abi.h>
#include <cilk/cilk_api.h>

#include <cstdio>
#include <cassert>
#include <cstring> // memset

struct frame_data {
  sp_node curr;
  sp_node cont;
  sp_node sync;
  unsigned char flags;
}; // struct frame_data

class shadow_stack : public utils::stack<frame_data> {
private:
  static constexpr unsigned char HELPER_MASK = 0x1;
public:
  enum class frame_type : unsigned char { SPAWNER = 0, HELPER = 1};
  void push(frame_type type = frame_type::SPAWNER)
  {
    assert(this->m_head != (uint32_t)-1);
    utils::stack<frame_data>::push();
    if (type == frame_type::SPAWNER) {
      assert(!(ancestor(1)->flags & HELPER_MASK));
      head()->flags = HELPER_MASK;
    }
  }
  void push_helper() { push(frame_type::HELPER); }
}; // class shadow_stack


// For parallel detection we will need this to be thread
// local. Actually, it will need to be a P-sized array since we need
// to access other workers' shadow stacks.
shadow_stack t_sstack;

// First cilk frame or steal
void init_strand() {
  assert(t_sstack.empty());
  t_sstack.push();
}

/// @TODO{Call cilk_tool_init/destroy automatically instead of from tsan}
void cilk_tool_init(void) {
  // For now, force to run sequentially
  __cilkrts_set_param("nworkers", "1");
}
void cilk_tool_destroy(void) { }

void cilk_enter_begin() {
  DBG_TRACE();
  disable_checking();
  t_sstack.push();
}
void cilk_enter_helper_begin(__cilkrts_stack_frame* sf,
                             void* this_fn, void* rip) {
  DBG_TRACE();
  disable_checking();

  t_sstack.push_helper();
}

void cilk_enter_end(__cilkrts_stack_frame *sf, void *rsp) {
  DBG_TRACE();

  if (t_sstack.empty()) {
    init_strand();
    // if (!check_enable_instrumentation) {
    //   check_enable_instrumentation = false;
    //   enable_instrumentation();
    //   stack_low_watermark = (uint64_t)(-1);
    // }
  }
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

  t_sstack.push_helper();
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
  assert(t_checking_disabled > 0);
  // We don't need to enable checking here b/c this worker will
  // immediately execute cilk_spawn_or_continue
}
