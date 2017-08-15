// Race detection class and public API
#include "rd.h"

// XXX: Not sure how to handle this yet
inline spbag* active() {
  spbag *f = t_sstack.head()->data.Sbag;
  assert(f);
  return f;
}

// On the one hand, we only ever want one race detector anyway, so it
// seems silly to use a class with all static data. On the other hand,
// this makes initialization slightly easier when using a static
// library.
class race_detector {
  static shadow_stack<reach::frame_data> t_sstack; // driver.cpp needs access
  //static bool TOOL_INITIALIZED = false;
  static uint64_t t_stack_low_watermark = (uint64_t)-1;
  static bool t_clear_stack = false;
  static shadow_mem g_smem;
  static reach g_reach;

  static enum rd_policy g_policy = RD_CONTINUE;
  static size_t g_num_races = 0;
  static bool g_checking_disabled = true;

  // originally this was set to false, but why?
  static bool t_checking_disabled = true;

  race_detector();
  ~race_detector();
  
  void reset() = delete; // re-enter Cilk after a break? stack_low_watermark?
  void print_states() = delete; // FILE *output = stdout

  static inline size_t num_races() { return g_num_races; }
  static inline enable_checking() { g_checking_disabled = false; }
  static inline void disable_checking() { g_checking_disabled = true; }
  static inline bool should_check() { return !g_checking_disabled; }

  // @todo{ Read environment variables for setting race-reporting policy.}
  static void set_policy(enum rd_policy p); // not thread-safe
  static void report_race(void* addr);
  static void check_access(bool is_read, void* rip, void* addr, size_t mem_size);
}; // struct race_detector
