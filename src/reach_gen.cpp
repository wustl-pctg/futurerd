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

bool general::precedes(node x, node y) {

  // x should definitely be in data
  assert(data.size() > x);

  // But sometimes we haven't used data[x] in a while
  //assert(data[x].size() > y);
  data[x].resize(data.size());

  return data[x][y];
}


void general::add_edge(node from, node to) {
  assert(data.size() > from);
  data[from].resize(data.size());
  data[from][to] = true;
}
