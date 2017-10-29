#include <cassert>

#include "bintree.hpp"
using key_t = bintree::key_t;
using node = bintree::node;
#include "bintree_par.tpp"

// bintree API methods
node* bintree::find(key_t k) { return find(m_root, k); }

node* bintree::insert(key_t k) {
  //node** slot = findpp(&m_root, &m_root, k);
  node** slot = findpp(&m_root, k);
  assert(slot);
  node* n = *slot;
  
  if (n) { // key already exists!
    assert(n->key == k);
    return n;
  }

  m_size++;
  return *slot = new node(k);
}

node* bintree::remove(key_t k) {
  //node** slot = findpp(&m_root, &m_root, k);
  node** slot = findpp(&m_root, k);
  assert(slot);
  node* n = *slot;

  // key doesn't exist anyway
  if (!n) return nullptr;

  if (!n->right)
    *slot = n->left; // done
  else {
    *slot = cutmin(n->right);
    (*slot)->left = n->left;
    (*slot)->right = n->right;
  }
  
  return n;
}

// Node methods, i.e. the real meat
node* bintree::cutmin(node* n) {
  assert(n);
  if (!n->left) return n;
  node* res = cutmin(n->left);
  if (res == n->left) n->left = nullptr;
  return res;
}

node* bintree::find(node* n, key_t k) {
  if (!n) return nullptr;
  if (n->key == k) return n;
  node* next = (k < n->key) ? n->left : n->right;
  return find(next, k);
}

node** bintree::findpp(node** slot, key_t k) {
  node* n = *slot;
  if (!n || n->key == k) return slot;
  node** next = (k < n->key) ? &n->left : &n->right;
  return findpp(next, k);
}

// sequential version, for reference
std::pair<node*,node*> bintree::seqsplit(key_t s, node* n) {
  if (!n)
    return {nullptr,nullptr};

  if (s < n->key) { // go left
    auto res = seqsplit(s, n->left);
    n->left = res.second; // "right" result
    return {res.first, n};
  } else { // go right
    auto res = seqsplit(s, n->right);
    n->right = res.first; // "left" result
    return {n, res.second};
  }
}

// Sequential version, for reference
node* bintree::seqmerge(node* t1, node* t2) {
  if (!t1) return t2;
  if (!t2) return t1;

  auto res = seqsplit(t1->key, t2);
  node* t2_left = res.first; node* t2_right = res.second;
  t1->left = seqmerge(t1->left, t2_left);
  t1->right = seqmerge(t2->right, t2_right);
  return t1;
}

void bintree::seqmerge(bintree* that) {
  this->m_root = seqmerge(this->m_root, that->m_root);
  that->m_root = nullptr;
  that->m_size = 0;
}

size_t bintree::validate(node* n) {
  if (!n) return 0;
  assert(!n->left || n->left->key <= n->key);
  assert(!n->right || n->right->key >= n->key);
  return validate(n->left) + validate(n->right) + 1;
}

size_t bintree::get_keys(node* n, key_t array[], size_t index) {
  if (!n) return index;
  index = get_keys(n->left, array, index);
  array[index++] = n->key;
  return get_keys(n->right, array, index);
  
}

void bintree::fprint_keys(FILE* f, node* n) {
  if (!n) return;
  fprint_keys(f, n->left);
  fprintf(f, "%i ", n->key);
  fprint_keys(f, n->right);
}
