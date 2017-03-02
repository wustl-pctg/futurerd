// Reachability data structure for series-parallel graphs

#include "om/om.hpp"

namespace sp {

struct node {
  om::ds::node* english;
  om::ds::node* hebrew;
};

class reachability {
private:
  static om::ds m_english;
  static om::ds m_hebrew;
  
public:
  static void insert(const sp::node* base, sp::node* inserted);
  static bool precedes(sp::node* x, sp::node* y);
  static bool logically_parallel(sp::node* x, sp::node* y);
  static bool sequential(sp::node* x, sp::node* y);
}; // class reachability

} // namespace sp
