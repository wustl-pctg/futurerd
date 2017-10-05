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
void futurerd_set_policy(enum rd_policy p);
size_t futurerd_num_races();
void futurerd_enable_checking();
void futurerd_disable_checking();
void futurerd_should_check();

} // extern C
