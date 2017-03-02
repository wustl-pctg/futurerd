// A single-level order-maintenance data structure implemented as a
// linked list. Inserts cannot fail.
#pragma once
#include "common.hpp"

namespace om {

template <class node = basic_node>
class list {
protected:
  node* m_head;
  static label_t next_label(node* base);
  
public:
  list(label_t initial_label = MIN_LABEL);
  ~list() {} // not necessary
  node* insert(node* base);

  static inline bool precedes(const node* x, const node* y)
  { return x->label < y->label; }
  
}; // class om_list


} // namespace om
