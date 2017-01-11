#include <cstdlib> // size_t

namespace futurerd {

typedef int strand_t; // XXX: temporary

enum DetectPolicy {
  ABORT = 0,
  CONTINUE = 1,
};

void set_policy(futurerd::DetectPolicy p);
size_t num_races();
void reset();
void init();
void destroy();

// extern inline void disable_checking() { __futurerd_disable_checking(); }
// extern inline void enable_checking() { __futurerd_enable_checking(); }

strand_t nt_out();
void nt_in(strand_t s);

}
