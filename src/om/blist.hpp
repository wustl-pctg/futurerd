// The bottom-level of an order-maintenance data structure,
// implemented as a linked list.
// Now inserts can fail, so we need to have a "split" operation.
// Inherits from om_list.
#include "list.hpp"
#include <cassert>
#include <cstddef>

namespace om {

// N / log_2 N = 2^58
static constexpr label_t NODE_INTERVAL = (((label_t)1) << 58);
static constexpr label_t SUBLIST_SIZE = ((label_t)64); // log_2 N = 64

template <class bot_ds>
class bl_node : basic_node {
  using tl_node = typename bot_ds::tl_node;
public:
  bot_ds* ds;
  bl_node(label_t lab, bot_ds* bot) : basic_node(lab), ds(bot) {}
  inline tl_node* above() const { return ds->above(); }
}; // class bl_node

// Just for convenience
template<class TLT> class blist;
template<class TLT>
using parent_list = list< bl_node< blist<TLT> > >;

template <class TLT>
class blist : public parent_list<TLT> {
  friend class bl_node<blist>; // for above
private:
  using tl_node = typename TLT::node;
  using node = bl_node<blist>;
  tl_node* m_above;
  
public:
  blist(tl_node* above, label_t initial_label = MIN_LABEL) :
    m_above(above), parent_list<TLT>(initial_label) {}
  inline tl_node* above() { return m_above; }
  
  node* insert(node* base) // can fail
  {
    assert(base);
    if (base->label == MAX_LABEL
        || (base->next && base->label + 1 == base->next->label))
      return nullptr; // no room

    return parent_list<TLT>::insert(base);
  }

  // Needs to return some kind of list of tl_nodes to insert
  tl_node* split();
}; // class blist

template <class TLT>
typename TLT::node* blist<TLT>::split()
{
  // size_t num_lists = 0;
  assert(this->m_head);

  tl_node* tl = above();
  tl_node* saved_next = tl->next;
  node* n = this->m_head;
  
  while (n) {
    label_t l = MIN_LABEL;
    for (size_t sz = 0; sz < SUBLIST_SIZE && n; sz++, n = n->next) {
      
    }
    

  }

}

} // namespace om
