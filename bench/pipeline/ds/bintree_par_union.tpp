// Parallel functions for a binary tree.

/** This is not a standalone file; hence the strange file
 *  extension. It must be included by the binary tree implementation
 *  file. It's simply easier to read this way.
*/

#include <cilk/cilk.h>
#define spawn cilk_spawn
#define sync cilk_sync
#define parfor cilk_for

/** Wrappers **/

void bintree::merge(bintree* that) {
  m_root = merge(this->m_root, that->m_root);

  // The nodes may still have children pointers that are actually
  // futures, so we need to go through and finish everything...
  // Is there a way around this?
  finish_async(m_root);
  
  this->m_size += that->m_size;
  that->m_root = nullptr;
  that->m_size = 0;
}

// node* bintree::async_merge(bintree* that) {
//   auto fake = new cilk::future<node*>();
//   fake->finish(reinterpret_cast<node*>(that->m_root));
//   return merge(m_root, fake);
// }

// void bintree::merge(bintree* that) {
//   node* new_root = async_merge(that);
//   // The nodes may now have future children pointers, so we need to
//   // touch everything to make sure it's done
//   finish_async(new_root);
//   m_root = reinterpret_cast<node*>(new_root);

//   that->m_root = nullptr;
//   that->m_size = 0;
// }

/** The real meat **/

#define FUTPTR(f) ((void*) (((uintptr_t)(f)) & ((uintptr_t)0x1))) != nullptr
#define GETPTR(f) (cilk::future<node*>*) ( ((uintptr_t)(f)) & ((uintptr_t)~0x1) )
#define SETPTR(loc,f) \
  (*(loc) = ((cilk::future<node*>*)(((uintptr_t)(f)) & ((uintptr_t)0x1))))

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
    result_right->put(n);

    cilk_pg_async(node*, new_result_right,
                  split, s, n->left, result_left, new_result_right);

    // Doesn't this actually need to happen before
    //result_right->put(n)? But then new_result_right hasn't been
    //created yet...manually allocate the future?
    //n->future_left = new_result_right;    
    SETPTR(&n->future_left, new_result_right);


  } else { // go right
    result_left->put(n);

    cilk_pg_async(node*, new_result_left,
                  split, s, n->right, new_result_left, result_right);

    // See above note.
    //n->future_right = new_result_left;
    SETPTR(&n->future_right, new_result_left);
    
  }
}

node* bintree::merge(node* t1, cilk::future<node*> *t2) {
  return merge(t1, t2->get());
}

node* bintree::merge(node* t1, node* t2) {
  if (!t1) return t2;
  if (!t2) return t1;

  cilk_pg_async2(node*, t2_left, t2_right,
                 split, t1->key, t2, t2_left, t2_right);

  // Note: we can't "get" t2_left here, since it may not be ready but
  // the other merge can start right away. So we spawn the first
  // merge, which may wait on t2_left, and also start the second
  // merge.

  // I think using spawn/sync doesn't change anything here, but I'm
  // not 100% sure. Do we actually need futures here? I have a version
  // laying around...
  t1->left = spawn merge(t1->left, t2_left);
  t1->right = merge(t1->right, t2_right);
  sync;
  
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
