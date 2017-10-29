// A simple btree. Actually, this is a 2-6 tree, but most of the code
// does not depend on that. This version is key only; no data.
#pragma once

class btree {
  using key_t = int;

  struct node {
    key_t keys[5];
    node* children[6];
  }; // struct node

  btree();
  ~btree();
  bool find(key_t k);
  node* insert(key_t k);
  node* remove(key_t k);
  void batch_insert(key_t array[], size_t size);
}; // class btree
