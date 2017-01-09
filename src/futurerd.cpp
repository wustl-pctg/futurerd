#include "futurerd.hpp"
#include "debug.hpp"

namespace futurerd {

static DetectPolicy g_policy = futurerd::DetectPolicy::ABORT;
static size_t g_num_races = 0;

void set_policy(DetectPolicy p) {
  if (p > 1) debug::die("Invalid Detection Policy\n");
  g_policy = p;
}

size_t num_races() { return g_num_races; }

}

// futurerd::detect_policy(futurerd::CONTINUE);
  //assert(futurerd::num_races() == 0);
