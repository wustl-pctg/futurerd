#include "futurerd.hpp"
#include "utils/union_find.hpp"
#include "sp_reach.hpp"
#include "shadow_stack.hpp"
#include "debug.hpp"

#define USE_CILK_API
#include <internal/abi.h>
#include <cilk/cilk_api.h>

#include <cstdio>

namespace futurerd {

typedef utils::uf::node set_t;

struct frame_data { sp_node curr, sync; };

// For parallel detection we will need this to be thread
// local. Actually, it will need to be a P-sized array since we need
// to access other workers' shadow stacks.
shadow_stack<frame_data> t_sstack;
sp_reachability g_sp;

/// XXX: Should be detector state object
static DetectPolicy g_policy = futurerd::DetectPolicy::ABORT;
static size_t g_num_races = 0;
//__thread set_t curr;

void set_policy(DetectPolicy p) {
  if (p > 1) debug::die("Invalid Detection Policy\n");
  g_policy = p;
}

size_t num_races() { return g_num_races; }

void init() {
  //___cilkrts_set_nworkers(1);
  __cilkrts_set_param("nworkers", "1");
  t_sstack.push();
  frame_data *f = t_sstack.head();
  f->curr = g_sp.first();
  f->sync = {0};
  // XXX: set f->curr to be initial english,hebrew nodes
}

void destroy() {
  assert(0); // not implemented yet
  // destroy/free all:
  //   reachability data structures/nodes
  //   shadow stack
}

void reset() { destroy(); init(); }
void print_stats(FILE *output = stdout) { }

void at_create(future_rd_info *info) { }

void at_finish(future_rd_info *info) { }

void at_get(future_rd_info *info) { }

void at_cilk_function() {
  t_sstack.push_spawner();
  frame_data *f = t_sstack.head();
  frame_data *p = t_sstack.ancestor(1);
  *f = *p;
}

// f is the new frame
void at_spawn() {
  t_sstack.push_helper();
  frame_data *f = t_sstack.head();
  frame_data *p = t_sstack.ancestor(1);

  if (!p->sync.english) // first of spawn group, set sync node
    g_sp.insert_sequential(&p->curr, &p->sync);


  sp_node tmp;
  g_sp.fork(&p->curr, &f->curr, &tmp);
  p->curr = tmp;
  f->sync = {0};
}

void at_leave_begin() { t_sstack.pop(); }

bool at_leave_end() { return t_sstack.size() == 1; }

void at_continuation() { t_sstack.pop(); }

void at_sync() {
  frame_data *f = t_sstack.head();
  f->curr = f->sync;
  f->sync = {0};
}

} // namespace futurerd
