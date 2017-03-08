#include "two_list.hpp"
#include <cassert>

namespace om {

using node = two_list::node;

two_list::two_list() {
  m_tl = new top_level();
  m_first_bl = new bot_level(m_tl->first());
}

node* two_list::insert(node* base)
{
  node* n = nullptr;

  while (!(n = base->ds->insert(base)))
    m_tl->insert_many(base->ds->split());

  return n;
}

bool two_list::precedes(const node* x, const node* y)
{
  assert(x && y);
  if (x->ds == y->ds) return bot_level::precedes(x,y);
  return top_level::precedes(x->above(),y->above());
}

} // namespace om
