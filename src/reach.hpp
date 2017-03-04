// General reachability data sructure

#include <cstddef>
#include <vector>

class reachability {
public:
  using node = std::size_t;
  void add_edge(node from, node to);
  bool precedes(node x, node y) const;
  
private:

  /// @todo{Use of std::vector for the reachability matrix will
  /// probably need to be rewritten for performance.}
  std::vector< std::vector<bool> > data;

}; // class reachability
