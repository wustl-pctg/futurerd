//#include "futurerd.h"
#include <cstdlib>
namespace futurerd {

enum DetectPolicy {
  ABORT = 0,
  CONTINUE = 1,
};


void set_policy(futurerd::DetectPolicy p);
size_t num_races();
// extern inline void disable_checking() { __futurerd_disable_checking(); }
// extern inline void enable_checking() { __futurerd_enable_checking(); }

}

// futurerd::detect_policy(futurerd::CONTINUE);
  //assert(futurerd::num_races() == 0);
