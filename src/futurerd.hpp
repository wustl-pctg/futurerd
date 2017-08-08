#include "spbag.hpp"
#include "shadow_mem.hpp"

#include <cstdlib> // size_t

namespace futurerd {

enum DetectPolicy {
  ABORT = 0,
  CONTINUE = 1, // *print* and continue
  SILENT = 2,
  END_POLICY,
};

struct future_rd_info {
  spbag* put_strand;
}; // struct future_rd_info

extern shadow_mem g_shadow_mem;
extern /*__thread*/ bool t_clear_stack;

// Public API
void set_policy(futurerd::DetectPolicy p);
size_t num_races();
void enable_checking();
void disable_checking();
bool should_check();

void check_access(bool is_read, void* rip, void* addr, size_t mem_size);

// Tool API (for cilktool and tsan)
void reset();
void init();
void destroy();

void at_create(future_rd_info*);
void at_put(future_rd_info*);
void at_finish(future_rd_info*);
void at_get(future_rd_info*);
// at_start(future_rd_info*); // not yet used

// returns whether or not we're at the first frame
bool at_cilk_function();
void at_leave_begin();

// Returns whether or not we're leaving the last frame
bool at_leave_end();

void at_sync();
void at_spawn();
void at_continuation();

} // namespace futurerd
