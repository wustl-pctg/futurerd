#include "sp_reach.hpp"
#include <cassert>

void sp_reachability::insert(const sp_node* base, sp_node* inserted)
{
  assert(inserted);
  
  inserted->english = m_english.insert(base->english);
  assert(inserted->english);
  
  inserted->hebrew = m_hebrew.insert(base->hebrew);
  assert(inserted->hebrew);
}

bool sp_reachability::precedes(sp_node* x, sp_node* y) const
{
  return m_english.precedes(x->english, y->english)
    && m_hebrew.precedes(x->hebrew, y->hebrew);
}

bool sp_reachability::logically_parallel(sp_node* x, sp_node* y) const
{
  // slightly faster than calling precedes twice
  bool prec_in_english = m_english.precedes(x->english, y->english);
  bool prec_in_hebrew = m_hebrew.precedes(x->hebrew, y->hebrew);

  return prec_in_english != prec_in_hebrew;
}

bool sp_reachability::sequential(sp_node* x, sp_node* y) const
{
  return !logically_parallel(x,y);
}
