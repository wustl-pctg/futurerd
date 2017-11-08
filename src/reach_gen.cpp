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

// merge 'this_node' into 'that_node'
void general::merge_nodes(node this_node, node that_node) {
  assert(data.size() > this_node);
  assert(data.size() > that_node);
  assert(data[that_node].size() <= data.size());
  data[that_node].resize(data.size());

  // copy every outgoing edges from this_node into that_node
  for(int i=0; i < data[this_node].size(); i++) {
    if(data[this_node][i] == true) {
      data[that_node][i] = true;
    }
  }

  // copy every incoming edges going into this_node to that_node
  for(int i=0; i < data.size(); i++) {
    if(data[i][this_node] == true) {
      data[i][that_node] = true;
    }
  }
}
