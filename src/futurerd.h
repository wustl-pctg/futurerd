#include <assert.h>

typedef unsigned int strand_t;
__thread strand_t t_strand;
__thread strand_t C;
__thread strand_t S;

/// @TODO
strand_t new_set() {}
strand_t new_strand() { }
strand_t add_edge() { }
// union-find?
void merge(strand_t x, strand_t y) {}


// extern "C" {

void tool_init() {}
void tool_print() {}
void tool_destroy() {}

// spawn
void detach_begin(__cilkrts_stack_frame* sf)
{
  // How does it work to merge them both, but keep them separate if we
  // need to split later?
  merge(t_strand, C);
  merge(t_strand, S);

  // Save values
  sf->old_C = C;
  sf->old_S = S;
  sf->left_S = S = new_set();

  t_strand = new_strand();
  merge(t_strand, C);
  merge(t_strand, S);
}

// how to get this from cilk tool?
void continuation_begin(__cilkrts_stack_frame* sf)
{
  C = sf->old_C;
  sf->right_S = S = new_set();
  t_strand = new_strand();
  merge(t_strand, C);
  merge(t_strand, S);
}

void sync_begin(__cilkrts_stack_frame* sf)
{

}

