// General reachability data structure

#include <cstddef>
#include <vector>

#include "utils/bitset.hpp"


namespace reach {
class general {
public:
  using node = std::size_t;

  general();
  ~general();

  void add_edges_from(node* from_nodes, int num_from, node to);
  void add_edge(node from, node to);
  node add_node();
  bool precedes(node x, node y);
  void merge_nodes(node this_node, node that_node);

private:

  // Reverse reachability: data[i][j] == true means we can reach i
  // from j. (i <- j) This is for better cache usage in add_edge.
  // std::vector< std::vector<bool> > data;
  std::vector<bitset> data;
}; // class reachability

} // namespace reach
