// Union-find data structure
// Pointer-based, for now. Does it matter if I use array-based
// instead?
#include "union_find.hpp"
#include "chunked_list.hpp"

//#include <cstdio>

namespace utils {

namespace uf {

std::size_t node::global_index = 0;

node* link(node* x, node* y) {
  if (x == y) return x;
  if (y->rank > x->rank) return link(y,x);

  // guaranteed that x has >= rank than y
  y->parent = x;
  if (x->rank == y->rank) ++x->rank;
  return x;
}

node* find(node* x)
{
  /// cilksan does this by using a list rather than recursion, which
  /// may be faster.
  if (x->parent != x) 
    return x->parent = find(x->parent);
  return x->parent;
}

node* merge(node* x,  node* y) { return link(find(x), find(y)); }

} // namespace uf

} // namespace utils
