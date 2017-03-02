#include "common.hpp"
#include "blist.hpp"
#include "tlist.hpp"

namespace om {

class two_list {
private:
  using top_level = tlist;
  using bot_level = blist<top_level>;

  top_level *m_tl;

public:
  using node = bl_node<bot_level>;

  two_list() {}
  ~two_list() {}  // not yet necessary
  node* insert(node* base);
  static bool precedes(const node* x, const node* y);
}; // class two_list


} // namespace om
