// Reachability data structure for series-parallel graphs

#include "om/om.hpp"

namespace sp {

struct node {
  om::node* english;
  om::node* hebrew;
};

class reachability {
private:
  om::ds m_english;
  om::ds m_hebrew;
  
public:
  sp::node* insert(sp::node* base);
  static bool precedes(sp::node* x, sp::node* y);
  static bool logically_parallel(sp::node* x, sp::node* y);
  static bool sequential(sp::node* x, sp::node* y);
}; // class reachability

} // namespace sp
