// Union-find data structure
// Pointer-based, for now. Does it matter if I use array-based
// instead?

#include <ChunkedList.hpp>

//namespace uf {

struct ufnode {
  static std::size_t global_index = 0;
  static ChunkedList<ufnode> allocator;
  static void* operator new(std::size_t sz)
  {
    return (void*) allocator.getNext();
  }
  static void* operator new[](std::size_t sz) = delete;

  std::size_t id;
  std::size_t size;
  ufnode* parent = nullptr;
  ufnode() : m_id(global_index++) {}
}; // struct ufnode

ufnode& union(ufnode& x, ufnode& y)
{
  if (y.size > x.size) return union(y,x);

  ufnode& root = find(x);
  ufnode& child = find(y);
  child.parent = root;
  root.size += child.size;
  return root;
}

ufnode& find(ufnode& x)
{
  if (x.parent != x) 
    return x.parent = find(x.parent);
  return x.parent;
}
  


  //} // namespace uf
