#include "futurerd.hpp"
#include "unionfind.hpp"
#include "debug.hpp"

#include <cstdio>

namespace futurerd {

static DetectPolicy g_policy = futurerd::DetectPolicy::ABORT;
static size_t g_num_races = 0;

void set_policy(DetectPolicy p) {
  if (p > 1) debug::die("Invalid Detection Policy\n");
  g_policy = p;
}

size_t num_races() { return g_num_races; }

void reset() {}
void init() {}
void destroy() {}
void print_stats(FILE *output = stdout) { }

strand_t nt_out() {
  auto reserved_node = new uf::node();
  auto reserved_id = reserved_node->id;

  // Other maintenance

  return reserved_id;
}

void nt_in(strand_t s) {

}




} // namespace futurerd
