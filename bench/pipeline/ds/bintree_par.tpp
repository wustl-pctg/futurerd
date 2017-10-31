// Parallel functions for a binary tree.

/** This is not a standalone file; hence the strange file
 *  extension. It must be included by the binary tree implementation
 *  file. It's simply easier to read this way.
*/

#include <cilk/cilk.h>
#define spawn cilk_spawn
#define sync cilk_sync
#define parfor cilk_for

#include <cstdio>

#define FUTPTR(f) (((void*) (((uintptr_t)(f)) & ((uintptr_t)0x1))) != nullptr)
#define GETPTR(f) ((cilk::future<node*>*) ( ((uintptr_t)(f)) & ((uintptr_t)~0x1) ))
#define SETPTR(loc,f) \
  (*(loc) = ((cilk::future<node*>*)(((uintptr_t)(f)) | ((uintptr_t)0x1))))
#define REPLACE(loc) \
  if (FUTPTR(*loc)) { \
    auto f = GETPTR(*loc); \
    *loc = f->get(); \
    delete f; \
  }

template <typename T>
struct future_ptr {
  using fut_t = cilk::future<T*>;
  union {
    T* data;
    fut_t* fut; // will have low bit set if in use
    uintptr_t raw;
  };
  future_ptr& operator=(fut_t* f) {
    fut = f;
    raw |= 0x1;
    return *this;
  }
  operator T*() {
    if (raw & 0x1) {
      auto f = reinterpret_cast<fut_t*>(raw & ~0x1);
      data = f->get();
      delete f; // not correct if allocated on stack...
    }
    return data;
  }
  fut_t* operator->() { return reinterpret_cast<fut_t*>(raw & ~0x1); }
}; // struct future_ptr

struct fnode {
  key_t key;
  future_ptr<fnode> left;
  future_ptr<fnode> right;
  fnode() = delete;
  fnode(key_t k) : key(k) {}
};
static_assert(sizeof(fnode) == sizeof(node), "Nodes aren't the same size!");

/** Wrappers **/

void bintree::merge(bintree* that) {
  m_root = merge(this->m_root, that->m_root);

  // The nodes may still have children pointers that are actually
  // futures, so we need to go through and finish everything...
  // Is there a way around this?
  //finish_async(m_root);
  
  this->m_size += that->m_size;
  that->m_root = nullptr;
  that->m_size = 0;
}

/** The real meat **/

namespace aux {
void split(key_t s, fnode* n,
           future_ptr<fnode> result_left,
           future_ptr<fnode> result_right) {
  
  if (!n) {
    result_left->put(nullptr);
    result_right->put(nullptr);
    return;
  }

  if (s < n->key) { // go left
    auto left = n->left;
    n->left = new cilk::future<fnode*>();
    result_right->put(n);
    split(s, left, result_left, n->left);
    n->left->finish();
  } else { // go right
    auto right = n->right;
    n->right = new cilk::future<fnode*>();
    result_left->put(n);
    split(s, right, n->right, result_right);
    n->right->finish();
  }
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

  if (s < n->key) { // go left

    REPLACE(&n->left);
    
    auto left = n->left;
    auto new_result_right  = new cilk::future<node*>();
    SETPTR(&n->future_left, new_result_right);
    result_right->put(n);
    split(s, left, result_left, new_result_right);
    new_result_right->finish();

    // result_right->put(n);
    // cilk_pg_async(node*, new_result_right,
    //               split, s, n->left, result_left, new_result_right);

    // Doesn't this actually need to happen before
    //result_right->put(n)? But then new_result_right hasn't been
    //created yet...manually allocate the future?
    //n->future_left = new_result_right;    
    // SETPTR(&n->future_left, new_result_right);

  } else { // go right

    REPLACE(&n->right);
    auto right = n->right;
    auto new_result_left = new cilk::future<node*>();
    SETPTR(&n->future_right, new_result_left);
    result_left->put(n);
    split(s, right, new_result_left, result_right);
    new_result_left->finish();
    
    // result_left->put(n);
    // cilk_pg_async(node*, new_result_left,
    //               split, s, n->right, new_result_left, result_right);

    // See above note.
    // n->future_right = new_result_left;
    // SETPTR(&n->future_right, new_result_left);
  }

}

node* bintree::merge(node* t1, cilk::future<node*> *t2) {
  return merge(t1, t2->get());
}

namespace aux {
// node* wait(node* n) {
//   if (!n) return nullptr;
//   if (FUTPTR(n)) return wait(GETPTR(n)->get());
//   if (FUTPTR(n->left)) n->left = wait(n->left);
//   if (FUTPTR(n->right)) n->right = wait(n->right);
//   return n;
// }

node* merge(node*,fnode*);
node* merge(node* t1, cilk::future<fnode*> *t2) {
  return merge(t1, t2->get());
}

void wait(node* n) {
  if (!n) return;
  assert(!FUTPTR(n));
  
  if (FUTPTR(n->left)) {
    REPLACE(&n->left);
    wait(n->left);
  }
  if (FUTPTR(n->right)) {
    REPLACE(&n->right);
    wait(n->right);
  }
}

// basically, conversion to node* ...
node* wait(fnode* n) {
  return nullptr; // todo
}

node* merge(node* t1, fnode* t2) {
  if (!t1) { /* need to wait*/ return aux::wait(t2); }
  if (!t2) return t1;

  future_ptr<fnode> t2_left = new cilk::future<fnode*>();
  future_ptr<fnode> t2_right = new cilk::future<fnode*>();
  aux::split(t1->key, t2, t2_left, t2_right);
  t2_left->finish(); t2_right->finish();

  t1->left = aux::merge(t1->left, t2_left);
  t1->right = aux::merge(t1->right, t2_right);

  return t1;
  
}

}

node* bintree::merge(node* t1, node* t2) {
  if (!t1) { aux::wait(t2); return t2; }
  if (!t2) { aux::wait(t1); return t1; }

  // cilk_pg_async2(node*, t2_left, t2_right,
  //                split, t1->key, t2, t2_left, t2_right);
  auto t2_left = new cilk::future<node*>();
  auto t2_right = new cilk::future<node*>();
  split(t1->key, t2, t2_left, t2_right);
  t2_left->finish(); t2_right->finish();

  // Note: we can't "get" t2_left here, since it may not be ready but
  // the other merge can start right away. So we spawn the first
  // merge, which may wait on t2_left, and also start the second
  // merge.

  // I think using spawn/sync doesn't change anything here, but I'm
  // not 100% sure. Do we actually need futures here? I have a version
  // laying around...
  t1->left = /*spawn*/ merge(t1->left, t2_left);
  //validate(t1->left);
  t1->right = merge(t1->right, t2_right);
  //validate(t1->right);
  //sync;
  
  return t1;
}

void bintree::finish_async(node* n) {
  if (!n) return;
  
  if (FUTPTR(n->future_left)) {
    cilk::future<node*>* f = GETPTR(n->future_left);
    n->left = f->get();
    delete f;
  }
  
  if (FUTPTR(n->future_right)) {
    cilk::future<node*>* f = GETPTR(n->future_left);
    n->right = f->get();
    delete f;
  }
  
  finish_async(n->left);
  finish_async(n->right);
}
