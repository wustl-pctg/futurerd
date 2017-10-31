// An simple binary tree. This verison is key only; no data.
#pragma once
#include <utility>

#include <future.hpp>
// template<typename T>
// struct future_ptr {
//   union {
//     T* data;
//     future<T*>* fut; // will have low bit set if in use
//   };
//   future_ptr(T* d = nullptr) : data(d) {}
//   future_ptr(future<T*>* f) : fut(f) {}
//   T* set(future<T*>* f) {
//     T* d = data;
//     fut = f;
//     fut |= 0x1;
//     return d;
//   }
//   T& operator->() const {
//     if ((fut & 0x1) == 0x0) return *T;
//     future<T*>* f = fut & ~0x1;
//     data = f->get();
//     return *data;
//   }
//   T& operator*() const {
//     if ((fut & 0x1) == 0x0) return *T;
//     future<T*>* f = fut & ~0x1;
//     data = f->get();
//     return *data;
//   }
// }; // struct future_ptr

class bintree {
public:
  using key_t = int;

  struct node {
    key_t key;
    node* left = nullptr;
    node* right = nullptr;
    cilk::future<node*>* future_left = nullptr;
    cilk::future<node*>* future_right = nullptr;
    node() = delete;
    node(key_t k) : key(k) {}
  }; // struct node
  //using node_ptr = future_ptr<node>;

  static node* cutmin(node* n);
  static node* find(node* n, key_t k);
  static node** findpp(node** slot, key_t k);
  
  static node* seqmerge(node* t1, node* t2);
  static std::pair<node*,node*> seqsplit(key_t s, node* n);
  static node* merge(node* t1, cilk::future<node*>* ft2);
  static void split(key_t s, node* n,
                    cilk::future<node*>* result_left,
                    cilk::future<node*>* result_right);
  static void finish_async(node* n);

  node* find(key_t k);
  node* insert(key_t k);
  node* remove(key_t k);
  node* async_merge(bintree* that);
  void merge(bintree* that);
  void seqmerge(bintree* that);
  static inline void merge(bintree* t1, bintree* t2) { t1->merge(t2); }
    
  
private:
  struct node *m_root = nullptr;
  size_t m_size = 0;

}; // class bintree
