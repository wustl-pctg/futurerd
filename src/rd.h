// Public API

enum rd_policy {
  RD_ABORT = 0,
  RD_CONTINUE = 1, // *print* and continue
  RD_SILENT = 2,
  RD_LAST_POLICY,
};

extern "C" {

#include <stddef.h> // size_t

// Not thread-safe
void set_policy(enum rd_policy p);
size_t num_races();
void enable_checking();
void disable_checking();
void should_check();

} // extern C
