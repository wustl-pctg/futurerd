/** An efficient Order Maintenance data structure. See the SP-Order
    paper and related. This is only meant to work sequentially.
*/
#pragma once

#include <cstdio> // FILE

#include "common.hpp"
#include "blist.hpp"
#include "tlist.hpp"

namespace om {

using top_level = tlist;
using bot_level = blist<top_level>;
using node = bl_node<bot_level>;

class two_level {
private:
  top_level *m_tl;

public:
  two_level();
  ~two_level() {} // not yet necessary
  node* insert(node* base);
}; // class two_level

node* two_level::insert(node* base)
{
  node* n = nullptr;

  while (!(n = base->ds->insert(base)))
    m_tl->insert_many(base->ds->split());

  return n;
}

//friend class bottom_level
bool precedes(const node* x, const node* y)
{
  assert(x && y);
  if (x->ds == y->ds) return bot_level::precedes(x,y);
  return top_level::precedes(x->above(),y->above());
}

} // namespace om
