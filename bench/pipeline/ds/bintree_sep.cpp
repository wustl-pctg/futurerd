#include <cassert>

#include "bintree2.hpp"
using key_t = bintree::key_t;
using node = bintree::node;

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

// Conceptually, this returns a pair....
void bintree::split(key_t s, node* n,
                    cilk::future<node*>* result_left,
                    cilk::future<node*>* result_right) {
  if (!n) {
    result_left->put(nullptr);
    result_right->put(nullptr);
    return;
  }

  //auto result = new future<node*>();
  if (s < n->key) { // go left
    result_right->put(n);

    // I think we have to do this manually...
    // cilk_async split(s, n->left, result_left, result);
    //result->start(); split(s, n->left, result_left, result); result->finish();
    if (n->future_left) {
      n->left = n->future_left->get();
      delete n->future_left;
      n->future_left = nullptr;
    }
    cilk_pg_async(node*, new_result_right,
                  split, s, n->left, result_left, new_result_right);
    n->future_left = new_result_right;

  } else { // go right
    result_left->put(n);

    if (n->future_right) {
      n->right = n->future_right->get();
      delete n->future_right;
      n->future_right = nullptr;
    }
    // cilk_async split(s, n->right, result, result_right);
    //result->start(); split(s, n->right, result, result_right); result->finish();
    cilk_pg_async(node*, new_result_left,
                  split, s, n->right, new_result_left, result_right);
    n->future_right = new_result_left;
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

node* bintree::merge(node* t1, cilk::future<node*> *ft2) {
  node* t2 = ft2->get();

  if (!t1) return t2;
  if (!t2) return t1;

  assert(!t1->future_left);
  assert(!t1->future_right);

  // std::pair<future<node*>,future<node*>> result = cilk_async split(t1->key, t2);
  cilk_pg_async2(node*, t2_left, t2_right,
                 split, t1->key, t2, t2_left, t2_right);

  // Ideally, you could just do this:
  //t1->left  = cilk_async merge(t1_left, t2_left);
  //t1->right = cilk_async merge(t1_right, t2_right);

  // But instead we'll use an ugly hack:
  create_auto_future(t1->future_left, merge, t1->left, t2_left);
  create_auto_future(t1->future_right, merge, t1->right, t2_right);
  return t1;
}

node* bintree::async_merge(bintree* that) {
  auto fake = new cilk::future<node*>();
  fake->finish(reinterpret_cast<node*>(that->m_root));
  return merge(reinterpret_cast<node*>(m_root), fake);
}

void bintree::finish_async(node* n) {
  if (!n) return;
  
  if (n->future_left) {
    n->left = n->future_left->get();
    delete n->future_left;
    n->future_left = nullptr;
  }

  if (n->future_right) {
    n->right = n->future_right->get();
    delete n->future_right;
    n->future_right = nullptr;
  }
  
  finish_async(n->left);
  finish_async(n->right);
}

void bintree::merge(bintree* that) {
  node* new_root = async_merge(that);
  // The nodes may now have future children pointers, so we need to
  // touch everything to make sure it's done
  finish_async(new_root);
  m_root = reinterpret_cast<node*>(new_root);

  that->m_root = nullptr;
  that->m_size = 0;
}

void bintree::seqmerge(bintree* that) {
  this->m_root = seqmerge(this->m_root, that->m_root);
  that->m_root = nullptr;
  that->m_size = 0;
}
