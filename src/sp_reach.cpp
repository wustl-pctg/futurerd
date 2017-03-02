#include "sp_reach.hpp"

namespace sp {

sp::node* reachability::insert(sp::node* base)
{
  sp::node* n = new sp::node();
  assert(n);
  
  n->english = m_english.insert(base->english);
  assert(n->english);
  
  n->hebrew = m_hebrew.insert(base->hebrew);
  assert(n->hebrew);
  
  return n;
}

static bool reachability::precedes(sp::node* x, sp::node* y)
{
  return m_english.precedes(x->english, y->english)
    && m_hebrew.precedes(x->hebrew, y->hebrew);
}

static bool reachability::logically_parallel(sp::node* x, sp::node* y)
{
  // slightly faster than calling precedes twice
  bool prec_in_english = m_english.precedes(x->english, y->english);
  bool prec_in_hebrew = m_hebrew.precedes(x->hebrew, y->hebrew);

  return prec_in_english != prec_in_hebrew;
}

static bool reachability::sequential(sp::node* x, sp::node* y)
{
  return !logically_parallel(x,y);
}




} // namespace sp
