#include "common.hpp"
#include "blist.hpp"
#include "tlist.hpp"

namespace om {

class two_list {
private:
  using top_level = tlist;
  using bot_level = blist<top_level>;

  // why pointers?
  top_level *m_tl;
  bot_level *m_first_bl;

public:
  using node = bl_node<bot_level>;

  two_list();
  ~two_list() {}  // not yet necessary
  node* insert(node* base);
  static bool precedes(const node* x, const node* y);

  inline node* first() const { return m_first_bl->first(); }
}; // class two_list


} // namespace om
