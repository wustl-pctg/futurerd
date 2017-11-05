#include "reach_gen.hpp"
#include <cassert>

using namespace reach;
using node = general::node;

// The 0th node is fake, for representing nodes in R
general::general() { add_node(); }

node general::add_node() {
  node id = data.size();
  data.push_back(std::vector<bool>(data.size()+1,false));
  data[id][id] = true;
  return id;
}

bool general::precedes(node x, node y) const {
  assert(data.size() > x);
  assert(data[x].size() > y);
  return data[x][y];
}


void general::add_edge(node from, node to) {
  assert(data.size() > from);
  data[from].resize(data.size());
  data[from][to] = true;
}
