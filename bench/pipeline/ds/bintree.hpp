// An simple binary tree. This verison is key only; no data.
#pragma once
#include <utility>
#include <cstdio>

#include <future.hpp>
// template<typename T>
// struct future_ptr {
//   union {
//     T* data;
//     cilk::future<T*>* fut; // will have low bit set if in use
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
//     cilk::future<T*>* f = fut & ~0x1;
//     data = f->get();
//     return *data;
//   }
//   T& operator*() const {
//     if ((fut & 0x1) == 0x0) return *T;
//     cilk::future<T*>* f = fut & ~0x1;
//     data = f->get();
//     return *data;
//   }
// }; // struct future_ptr

class bintree {
public:
  using key_t = int;

  struct node {
    key_t key;
    union {
    node* left = nullptr;
      cilk::future<node*>* future_left = nullptr;
    };
    union {
    node* right = nullptr;
      cilk::future<node*>* future_right = nullptr;
    };
    node() = delete;
    node(key_t k) : key(k) {}
  }; // struct node
  //using node_ptr = future_ptr<node>;

  //bintree(key_t array[], size_t size);

  // These *could* be member functions of node, but I prefer this
  // because...some reason.
  static node* cutmin(node* n);
  static node* find(node* n, key_t k);
  static node** findpp(node** slot, key_t k);
  static node* seqmerge(node* t1, node* t2);
  static std::pair<node*,node*> seqsplit(key_t s, node* n);

  // Sequential methods
  node* find(key_t k);
  node* insert(key_t k);
  node* remove(key_t k);
  void seqmerge(bintree* that);

  // Utility
  inline size_t size() const { return m_size; }
  inline void validate() const {
    size_t size = validate(m_root);
    assert(size == m_size);
  }
  inline void get_keys(key_t array[]) { get_keys(m_root, array, 0); }
  inline void print_keys() { fprint_keys(stdout); }
  inline void fprint_keys(FILE* f) {
    fprint_keys(f, m_root);
    fprintf(f, "\n");
  }
  static size_t validate(node* n);
  static size_t get_keys(node* n, key_t array[], size_t index);
  static void fprint_keys(FILE* f, node* n);

  // Parallel functions and methods
  void merge(bintree* that);
  static node* merge(node* t1, node* t2);
  static node* merge(node* t1, cilk::future<node*>* ft2);
  node* async_merge(bintree* that);
  static void split(key_t s, node* n,
                    cilk::future<node*>* result_left,
                    cilk::future<node*>* result_right);
  static void finish_async(node* n);

private:
  node* m_root = nullptr;
  size_t m_size = 0;

}; // class bintree
