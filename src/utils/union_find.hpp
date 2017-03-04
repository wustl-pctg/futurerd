namespace utils {

namespace uf {
struct node {
  static std::size_t global_index;
  static void* operator new(std::size_t sz);
  static void* operator new[](std::size_t sz) = delete;

  std::size_t id;
  std::size_t size;
  node* parent = nullptr;

  // Field for "attached" and "done"?
  
  node() : id(global_index++) {}
}; // struct node

node* merge(node* x,  node* y);
node* find(node* x);
} // namespace uf

} // namespace utils
