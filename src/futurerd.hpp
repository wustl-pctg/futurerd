#include <cstdlib> // size_t

namespace futurerd {

enum DetectPolicy {
  ABORT = 0,
  CONTINUE = 1, // *print* and continue
  SILENT = 2,
  END_POLICY,
};

struct future_rd_info {

}; // struct future_rd_info

// Public API
void set_policy(futurerd::DetectPolicy p);
size_t num_races();

// Fake, just for testing. Sets the memory location we want to check
// for races
void set_loc(void* addr);
void check_read(void *addr);
void check_write(void *addr);

// Tool API (for cilktool and tsan)
void reset();
void init();
void destroy();

void at_create(future_rd_info*);
void at_finish(future_rd_info*);
void at_get(future_rd_info*);
void at_put(future_rd_info*);

void at_cilk_function();
void at_leave_begin();

// Returns whether or not we're leaving the last frame
bool at_leave_end();

void at_sync();
void at_spawn();
void at_continuation();

} // namespace futurerd
