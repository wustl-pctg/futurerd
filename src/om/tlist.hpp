// The top-level of an order-maintenance data structure, implemented
// as a linked list.
#include "list.hpp"

namespace om {

using tl_node = om::basic_node;

class tlist : public list<tl_node> {
public:
  using node = tl_node;
  tlist() {}
  ~tlist() {} // not needed yet
  void relabel();
  void insert_many(tl_node* start);
}; // class tlist

#include <cassert>
// implementation
void tlist::relabel()
{
  // not implemented yet
  assert(0);
}

void tlist::insert_many(tl_node* start)
{
  // not implemented yet
  assert(0);
}


} // namespace om
