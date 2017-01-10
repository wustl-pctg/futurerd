// Union-find data structure
// Pointer-based, for now. Does it matter if I use array-based
// instead?
#include "ChunkedList.hpp"
#include "unionfind.hpp"

namespace uf {

struct node {
  static std::size_t global_index;
  static ChunkedList<node> allocator;
  static void* operator new(std::size_t sz)
  {
    return (void*) allocator.getNext();
  }
  static void* operator new[](std::size_t sz) = delete;

  std::size_t id;
  std::size_t size;
  node* parent = nullptr;
  node() : id(global_index++) {}
}; // struct node

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
