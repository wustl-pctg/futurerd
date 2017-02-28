// A single-level order-maintenance data structure implemented as a
// linked list. Inserts cannot fail.
#pragma once
#include "common.hpp"
#include <cassert>

namespace om {

template <class node = basic_node>
class list {
protected:
  node* m_head;
  // we can assume we have room
  static label_t next_label(node* base)
  {
    label_t succ_label = (base->next) ? base->next->label : MAX_LABEL;

    // Average them
    label_t label = (base->label / 2) + (succ_label / 2);

    // Correction for averaging two odd integers (integer division
    // rounding)
    /// @todo{Does this case ever happen?}
    if ((base->label & next_label & 0x1) == 0x1) label++;

    // Correction for adding to the end
    if (!base->next) label++;

    assert(label > base->label);
    assert(base->next == nullptr || label < base->next->label);

    return label;
  }
  
public:
  list(label_t initial_label = MIN_LABEL)
    : m_head(new node(initial_label))
  { assert(m_head); }
  ~list() {} // not necessary
  static inline bool precedes(const node* x, const node* y)
  { return x->label < y->label; }
  
  node* insert(node* base)
  {
    assert(base);

    // Just assert that there is room
    assert(base->label != MAX_LABEL);
    if (base->next) assert(!(base->label + 1 == base->next->label));

    node* n = new node(next_label(base));
    assert(n);
    splice(base, n);
    return n;
  }
  
}; // class om_list


} // namespace om
