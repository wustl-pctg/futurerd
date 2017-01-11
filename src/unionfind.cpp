// Union-find data structure
// Pointer-based, for now. Does it matter if I use array-based
// instead?
#include "unionfind.hpp"
#include "ChunkedList.hpp"

#include <cstdio>

static void* uf::node::operator new(std::size_t sz) {
  static ChunkedList<node> allocator;
  return (void*) allocator.getNext();
}

namespace uf {

std::size_t node::global_index = 0;

node* find(node* x)
{
  if (x->parent != x) 
    return x->parent = find(x->parent);
  return x->parent;
}

node* merge(node* x, node* y)
{
  if (y->size > x->size) return merge(y,x);

  node* root = find(x);
  node* child = find(y);
  child->parent = root;
  root->size += child->size;
  return root;
}

} // namespace uf
