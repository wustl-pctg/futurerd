#include <cassert>

#include "om.hpp"

namespace om {

tl_node* om_ds::get_tl(const node* n) const
{
  tl_node* tl = n->list->above();
  assert(tl && tl->level == om::MAX_LEVEL);
  return tl;
}

// Not yet implemented
void om_ds::relabel() { }

om_ds::om_ds() : m_root(new tl_node()), m_height(0)
{
  m_root->below = new bl_list(m_root);
  m_root->level = MAX_LEVEL;
  m_root->num_leaves = 1;
  m_root->label = 0;
  m_root->parent = m_root->left = m_root->right = nullptr;

  // m_head = m_tail = m_root;

  // Initial insert
}

om_ds::~om_ds()
{
  // I don't think we actually need this...
}

bl_node* om_ds::insert(node* base)
{
  bl_node* n = nullptr;

  // while (!(n = base->list->insert(base)))
  //   relabel();
  n = base->list->insert(base);

  // just for now
  assert(n);
  
  return n;
}

bool om_ds::precedes(const node* x, const node* y) const
{
  // if (x == NULL) return true;
  // if (y == NULL) return false;
  assert(x && y);
  if (x->list == y->list) return x->label < y->label;
  return get_tl(x)->label < get_tl(y)->label;
}

void om_ds::fprint(FILE* out) const
{

}

void om_ds::verify()
{

}

} // namespace om
