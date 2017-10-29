#include "treap.hpp"
#include <cstdio>
#include <cassert>
#include <cstdlib>

using key_t = treap::key_t;
using priority_t = treap::priority_t;
using node = treap::node;

// Private helper functions
inline priority_t treap::new_priority() {
  return m_prio_min + rand() % (m_prio_max - m_prio_min + 1);
}

/*
    a    <--      b
   / \           / \
  b   Z         X   a
 / \               / \
X   Y             Y   Z
*/
node* rotate_left(node* b) {
  node *a = b->right;
  b->right = a->left;
  a->left = b;
  return a;
}

/*
    a    -->      b
   / \           / \
  b   Z         X   a
 / \               / \
X   Y             Y   Z
*/
node* rotate_right(node* a) {
  node *b = a->left;
  a->left = b->right;
  b->right = a;
  return b;
}

node* treap::find(key_t k) { return find(m_root, k); }

node* treap::insert(key_t k) {
  // a little inefficient...
  node* result = find(k);
  if (!result) {
    result = new node(k, new_priority());
    m_root = insert(m_root, result);
    m_size++;
  }
  return result;
}
node* treap::remove(key_t k) {
  // again, inefficient...
  node* result = find(k);
  if (result) {
    m_root = remove(m_root, k);
    result->left = result->right = nullptr;
    m_size--;
  }
  return result;

}

node* treap::find(node* n, key_t k) {
  if (!n) return nullptr;
  if (k == n->key) return n;
  node* next = (k < n->key) ? n->left : n->right;
  return find(next, k);
}

node* treap::insert(node* root, node* n) {
  if (!root) return n;
  assert(root->key != n->key);

  if (n->key < root->key) { // go left
    root->left = insert(root->left, n);
    if (root->left->priority < root->priority)
      root = rotate_right(root);
  } else { // go right
    root->right = insert(root->right, n);
    if (root->right->priority < root->priority)
      root = rotate_left(root);
  } 
  return root;
}

node* treap::remove(node* root, key_t k) {
  assert(root);
  if (root->key == k) {

    // If both children are null, return null
    if (!root->left && !root->right) return nullptr;

    // if exactly one child is null, return the other
    if (!root->left) return root->right;
    if (!root->right) return root->left;

    // If both non-null, do a rotate then recurse
    if(root->left->priority < root->right->priority) {
        root = rotate_right(root);
        root->right = remove(root->right, k);
    } else {
        root = rotate_left(root);
        root->left = remove(root->left, k);
    }
    return root;
  }

  if (k < root->key) { // go left
    root->left = remove(root->left, k);
  } else { // go right
    root->right = remove(root->right, k);
  }
  return root;
}

