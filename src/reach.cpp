#include "reach.hpp"

bool reachability::precedes(node x, node y) const
{
  return data[x][y];
}


void reachability::add_edge(node from, node to)
{
  data[from][to] = true;
}
