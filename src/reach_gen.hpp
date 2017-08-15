// General reachability data sructure

#include <cstddef>
#include <vector>

namespace reach {
class general {
public:
  //struct sframe_data {};
  using node = std::size_t;
  void add_edge(node from, node to);
  node add_node();
  bool precedes(node x, node y) const;
  
private:
  static size_t global_index = 1; // 0 is fake, for representing nodes
                                  // not in R
  
  /// @todo{Use of std::vector for the reachability matrix will
  /// probably need to be rewritten for performance.}
  std::vector< std::vector<bool> > data;
}; // class reachability

} // namespace reach