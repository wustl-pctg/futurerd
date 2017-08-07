#include "reach.hpp"

node add_node() {
  return global_index++;
  // Nothing else is necessary; resizing will be done automatically
  // while accessing the vector in precedes() or add_edge().
}

bool reachability::precedes(node x, node y) const {
  return data[x][y];
}


void reachability::add_edge(node from, node to) {
  data[from][to] = true;
}
