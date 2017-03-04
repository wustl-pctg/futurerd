// Reachability data structure for series-parallel graphs

// For now we're just going to use the two_list version
#include "om/om.hpp"

struct sp_node {
  om::ds::node* english;
  om::ds::node* hebrew;
};

class sp_reachability {
private:
  om::ds m_english;
  om::ds m_hebrew;
  
public:
  void insert(const sp_node* base, sp_node* inserted);
  bool precedes(sp_node* x, sp_node* y) const;
  bool logically_parallel(sp_node* x, sp_node* y) const;
  bool sequential(sp_node* x, sp_node* y) const;
}; // class sp_reachability
