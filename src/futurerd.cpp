#include "futurerd.hpp"
#include "shadow_stack.hpp"
#include "debug.hpp"

#define USE_CILK_API
#include <internal/abi.h>
#include <cilk/cilk_api.h>

#include <cstdio>

namespace futurerd {

// Actually, I think I may only need the Pbag, and just to keep a
// thread-local "active" Sbag...
struct frame_data {
  spbag *Sbag, *Pbag;
  //  spbag *active; // do we need this? Would it just be head()->Sbag?
};

struct access_info {
  spbag *last_reader;
  spbag *last_writer;
};

// For parallel detection we will need this to be thread
// local. Actually, it will need to be a P-sized array since we need
// to access other workers' shadow stacks.
/*__thread*/ shadow_stack<frame_data> t_sstack;
shadow_mem g_shadow_mem;

/*static*/ /*__thread*/ bool t_clear_stack = false;

/// XXX: Should be detector state object
static DetectPolicy g_policy = futurerd::DetectPolicy::ABORT;
static size_t g_num_races = 0;

// originally this was set to false, but why?
static /*__thread*/ bool t_checking_disabled = true;

void set_policy(DetectPolicy p) {
  if (p >= DetectPolicy::END_POLICY)
    debug::die("Invalid Detection Policy\n");
  g_policy = p;
}

size_t num_races() { return g_num_races; }

//__attribute__((always_inline))
void enable_checking() {
  //assert(t_checking_disabled);
  t_checking_disabled = false;
}

//__attribute__((always_inline))
void disable_checking() {
  //assert(!t_checking_disabled);
  t_checking_disabled = true;
}

//[[gnu::always_inline]] static
bool should_check() { return !t_checking_disabled; }
//{ return(g_instr_enabled && t_checking_disabled == 0); }

inline spbag* active() {
  spbag *f = t_sstack.head()->data.Sbag;
  assert(f);
  return f;
}

// Later on, report what kind of race, i.e. RW, WR, or WW
void report_race(void* addr) {
  if (! (g_policy == futurerd::DetectPolicy::SILENT) )
    fprintf(stderr, "Race detected at %p\n", addr);
  g_num_races++;
  if (g_policy == futurerd::DetectPolicy::ABORT)
    std::abort();
}

void check_access(bool is_read, void* rip, void* addr, size_t mem_size) {
  shadow_mem::addr_info_t *slot = g_shadow_mem.find((uint64_t)addr);

  // no previous accesses
  if (slot == nullptr) {
    g_shadow_mem.insert(is_read, (uint64_t)addr, active(), (uint64_t)rip);
    return;
  }
  
  // check race with last writer
  if (slot->last_writer.access != nullptr // if last writer exists
      && !slot->last_writer.access->precedes_now())
    report_race(addr);
  
  // if write, check race with last read
  if (!is_read // if a write
      && slot->last_reader.access != nullptr // and last reader exists
      && !slot->last_reader.access->precedes_now()) {
    report_race(addr);
  }
  
  // update shadow mem
  g_shadow_mem.update(slot, is_read, (uint64_t)addr, active(), (uint64_t)rip);
}

// either initialization, spawn or future creation
void new_function() {
  frame_data *f = &t_sstack.head()->data;
  f->Sbag = new spbag(spbag::bag_kind::S);
  f->Pbag = nullptr; // will create this lazily when we need it
  //f->active = f->Sbag;
}


void init() {
  //___cilkrts_set_nworkers(1);
  __cilkrts_set_param("nworkers", "1");
  // t_sstack.push();
  // new_function();
}

void destroy() {
  assert(0); // not implemented yet
  // destroy/free all:
  //   reachability data structures/nodes
  //   shadow stack
}

// This should do things like settings stack_low_watermark. Things
// that need to be reset whenever we re-enter Cilk after a break.
void reset() { }
void print_stats(FILE *output = stdout) { }

/// TODO: some duplication between this and at_spawn
void at_create(future_rd_info *info) {
  // very similar to spawn
  t_sstack.push_future();
  new_function();
}

// Returning from a spawned function or future function
void at_continuation() {
  frame_data *f = &t_sstack.head()->data;

  // XXX: can't we just change f->Sbag to be a bag, then assign it to p->Pbag?
  frame_data *p = &t_sstack.parent()->data;

  // if (!p->Pbag) p->Pbag = new spbag(spbag::bag_kind::P);
  // else p->Pbag->set_kind(spbag::bag_kind::P);
  // p->Pbag->merge(f->Sbag);

  if (!p->Pbag) 
    p->Pbag = f->Sbag;
  else
    p->Pbag->merge(f->Sbag);
  // sometimes this bag may have been merged with an S-bag, so it
  // think it's an Sbag. But now it must be a Pbag. See basic/test6.
  // XXX: is there a cleaner way to do this?
  spbag::find(p->Pbag)->set_kind(spbag::bag_kind::P);
  //p->Pbag->set_kind(spbag::bag_kind::P);

  // XXX: can't we just change f->Sbag to be a Pbag, rather than
  // creating a new bag?
  // if (!f->Pbag) f->Pbag = new spbag(spbag::bag_kind::P);
  // f->Pbag->merge(f->Sbag);
}

void at_put(future_rd_info *info) {
  assert(0); // implement me!
}

// similar to at_leave_begin
void at_finish(future_rd_info *info) {
  at_continuation();
  assert(t_sstack.in_future_helper());

  // when using fake futures, no cilk_leave_frame is called.
  t_clear_stack = true;

  //info->put_strand = t_sstack.head()->data.Pbag;
  t_sstack.pop();
  info->put_strand = t_sstack.head()->data.Pbag;


}

void at_get(future_rd_info *info) {
  assert(info->put_strand);
  t_sstack.head()->data.Sbag->merge(info->put_strand);
}

// A sync is equivalent to calling "get" on all the futures that were
// "spawned" in this block
void at_sync() {
  frame_data *f = &t_sstack.head()->data;

  // make sure it's a real sync
  if (!f->Pbag) return; 

  f->Sbag->merge(f->Pbag);
  f->Pbag = nullptr;

}

bool at_cilk_function() {
  t_sstack.push_spawner();
  frame_data *f = &t_sstack.head()->data;
  if (__builtin_expect(t_sstack.size() > 1, true)) {

    // I *think* instead I should just keep a thread-local "active"
    // strand...
    frame_data *p = &t_sstack.parent()->data;
    //*f = *p;
    f->Sbag = p->Sbag;
    
    return false;
  } else {
    f->Sbag = new spbag(spbag::bag_kind::S);
    f->Pbag = nullptr; // will create this lazily when we need it
    return true;
  }
}

// f is the new frame
void at_spawn() {
  t_sstack.push_helper();
  new_function();
}

void at_leave_begin() {
  if (t_sstack.in_spawn_helper()) {

    at_continuation();

    // This is the point where we need to set flag to clear accesses to stack 
    // out of the shadow memory.  The spawn helper of this leave_begin
    // is about to return, and we want to clear stack accesses below 
    // (and including) spawn helper's stack frame.  Set the flag here
    // and the stack will be cleared in tsan_func_exit.
    t_clear_stack = true;
    
  } else if (t_sstack.size() > 1) {

    // we may have strands saved in the parent's Pbag. In other words,
    // we may be unsynced.
    frame_data *f = &t_sstack.head()->data;
    frame_data *p = &t_sstack.parent()->data;

    assert(f->Sbag);
    p->Sbag = f->Sbag;

    if (f->Pbag) {
      assert(f->Pbag->kind == spbag::bag_kind::P);
      if (!p->Pbag) {
        p->Pbag = f->Pbag;
      } else {
        assert(p->Pbag->kind == spbag::bag_kind::P);
        p->Pbag->merge(f->Pbag);
      }
    }
    //t_sstack.parent()->data = t_sstack.head()->data;  
    // uf::merge(p->S, f->S);
  }
  // Copy to parent frame
  
  t_sstack.pop();
}

// Last frame was already popped in at_leave_begin()
bool at_leave_end() { return t_sstack.size() == 0; }

} // namespace futurerd
