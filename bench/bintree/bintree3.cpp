#include <cassert>

#include "bintree3.hpp"
//#include <cilk/cilk.h>
#define spawn //cilk_spawn
#define sync //cilk_sync

using key_t = bintree::key_t;
using node = bintree::node;
using fut_t = bintree::fut_t;
using futpair_t = bintree::futpair_t;

void bintree::merge(bintree *that) {
  this->m_size += that->m_size;
  m_root = merge(this->m_root, that->m_root);

  // TODO: Angelina claims this isn't necessary. I don't understand.
  // replace_all(m_root); // have to do this to "touch" every node

  that->m_root = nullptr;
  that->m_size = 0;
  delete that;
}

node* bintree::insert(node* n, const key_t k) {
  if (!n) return new node(k);
  if (k < n->key)
    n->left = insert(n->left, k);
  else
    n->right = insert(n->right, k);
  return n;
}

std::size_t bintree::validate(node *n) {
  if (n == nullptr) return 0;
  assert(n->key >= 0);
  assert(!n->left || n->left->key <= n->key);
  assert(!n->right || n->right->key >= n->key);
  return validate(n->left) + validate(n->right) + 1;
}

void bintree::get_key_counts(node* n, int *counts, key_t max_key) {
  if (n == nullptr) return;
  get_key_counts(n->left, counts, max_key);
  get_key_counts(n->right, counts, max_key);
  assert(n->key >= 0);
  assert(n->key <= max_key);
  counts[n->key]++;
}

// void bintree::replace_all(node *n) {
//   if(!n) return;
//   assert(!IS_FUTPTR(n->left));
//   assert(!IS_FUTPTR(n->right));
//   if(IS_FUTPTR(n->left)) { REPLACE(&n->left); }
//   if(IS_FUTPTR(n->right)) { REPLACE(&n->right); }
//   replace_all(n->left);
//   replace_all(n->right);
// }

void bintree::print_keys(node *n) {
  if(!n) return;
  print_keys(n->left);
  fprintf(stderr, "%p: %i, ", n, n->key);
  print_keys(n->right);
}

// We don't actually support put/get style, but this makes things clearer.
node* immediate(node *n) { return n; }
#define put(fut,res) reasync_helper<node*,node*>((fut), immediate, (res))
//(fut)->finish((res))
//
#define async_split(fut, args...) \
  reasync_helper<node*,node*,key_t,fut_t*,fut_t*>((fut), split, args)
  //split(args)
  //

node* bintree::split(node* n, key_t s, fut_t* res_left, fut_t* res_right) {
  assert(n);

  if (s < n->key) { // go left
    auto next = n->left;

    if (!next) {
      put(res_left, nullptr);
      // put(res_right, n); 
    } else { // lookahead

      auto next_res_right = (fut_t*) malloc(sizeof(fut_t));
      n->left = next_res_right;
      
      if (s < next->key) { // left-left case
        async_split(next_res_right, next, s, res_left, next_res_right);
      } else { // left-right case
        async_split(res_left, next, s, res_left, next_res_right);
      }
      // finish res_right by returning n.
    }
  } else { // go right
    auto next = n->right;

    if (!next) {
      //put(res_left, n);
      put(res_right, nullptr);
    } else { // lookahead

      auto next_res_left = (fut_t*) malloc(sizeof(fut_t));
      n->right = next_res_left;

      if (s < next->key) { // right-left case
        async_split(next_res_left, next, s, next_res_left, res_right);
      } else { // right-right case
        async_split(res_right, next, s, next_res_left, res_right);
      }
    }
  }
  return n;
}

futpair_t bintree::split2(node* n, key_t s) {
  auto left = (fut_t*) malloc(sizeof(fut_t));
  auto right = (fut_t*) malloc(sizeof(fut_t));

  // lookahead
  if (s < n->key)
    async_split(right, n, s, left, right);
  else
    async_split(left, n, s, left, right);

  return {left, right};
}


node* bintree::merge(node* lr, fut_t* rr) {
  auto res = merge(lr, rr->get());
  free(rr);
  return res;
}


node* bintree::merge(node* lr, node* rr) {
  //printf("Merge at %i and %i: ", lr->key, rr->key);
  if (!lr) return rr;
  if (!rr) return lr;

  auto res = split2(rr, lr->key);
  auto left = res.first;
  auto right = res.second;

  // printf("After split at %i:\n", lr->key);
  // print_keys(left->get()); printf("\n");
  // print_keys(right->get()); printf("\n");

  // Is it okay to just spawn here?
  lr->left  = spawn merge(lr->left, left);
  lr->right =       merge(lr->right, right);
  sync;

  //printf("End split at %i\n", lr->key);
  return lr;
}
