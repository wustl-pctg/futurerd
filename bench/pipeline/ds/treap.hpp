// Treap, no data
#pragma once
#include <cstddef>

class treap {
public:
  using key_t = int;
  using priority_t = int;

  struct node {
    key_t key;
    priority_t priority;
    node *left = nullptr;
    node *right = nullptr;

    node() = delete;
    node(key_t k);
    node(key_t k, priority_t p);
  }; // struct node

  treap() = delete;
  treap(priority_t prio_min, priority_t prio_max)
    : m_prio_min(prio_min), m_prio_max(prio_max) {}

  // Normal treap operations
  node* find(key_t k);
  node* insert(key_t k);
  node* remove(key_t k);

  // Parallel operations
  // static node* meld(node* t1, node* t2);
  // static node* diff(node* t1, node* t2);
  // void meld(treap* that);
  // void diff(treap* that);

private:
  priority_t new_priority();
  static node* find(node* n, key_t k);
  static node* insert(node* root, node* n);
  static node* remove(node* root, key_t k);
  static node* rotate_left(node* n);
  static node* rotate_right(node* n);
  
  node *m_root = nullptr;
  size_t m_size = 0;
  priority_t m_prio_min;
  priority_t m_prio_max;

}; // class treap
