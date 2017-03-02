#include "sp_reach.hpp"
#include <cassert>

namespace sp {

void reachability::insert(const sp::node* base, sp::node* inserted)
{
  assert(inserted);
  
  inserted->english = m_english.insert(base->english);
  assert(inserted->english);
  
  inserted->hebrew = m_hebrew.insert(base->hebrew);
  assert(inserted->hebrew);
}

bool reachability::precedes(sp::node* x, sp::node* y)
{
  return m_english.precedes(x->english, y->english)
    && m_hebrew.precedes(x->hebrew, y->hebrew);
}

bool reachability::logically_parallel(sp::node* x, sp::node* y)
{
  // slightly faster than calling precedes twice
  bool prec_in_english = m_english.precedes(x->english, y->english);
  bool prec_in_hebrew = m_hebrew.precedes(x->hebrew, y->hebrew);

  return prec_in_english != prec_in_hebrew;
}

bool reachability::sequential(sp::node* x, sp::node* y)
{
  return !logically_parallel(x,y);
}




} // namespace sp
