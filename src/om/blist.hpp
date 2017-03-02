// The bottom-level of an order-maintenance data structure,
// implemented as a linked list.
// Now inserts can fail, so we need to have a "split" operation.
// Inherits from om_list.
#include "list.hpp"

namespace om {

template <class bot_ds>
class bl_node : public basic_node {
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
  blist(tl_node* above, label_t initial_label = MIN_LABEL);
  inline tl_node* above() { return m_above; }
  
  node* insert(node* base); // can fail
  
  // Needs to return some kind of list of tl_nodes to insert
  tl_node* split();
}; // class blist


} // namespace om
