// Race detection class and public API
#include "rd.h"
#include "shadow_stack.hpp"
#include "reach_structured.hpp"
#include "shadow_mem.hpp"

// On the one hand, we only ever want one race detector anyway, so it
// seems silly to use a class with all static data. On the other hand,
// this makes initialization slightly easier when using a static
// library.
// XXX: template race_detector with reachability data structure?
class race_detector {
public:
  static shadow_stack<reach::structured::sframe_data> t_sstack;
  static uint64_t t_stack_low_watermark;
  static bool t_clear_stack;
  static shadow_mem g_smem;
  static reach::structured g_reach;

  static enum rd_policy g_policy;
  static size_t g_num_races;
  static bool g_checking_disabled;
  static bool t_checking_disabled;

  race_detector();
  ~race_detector();
  
  void reset() = delete; // re-enter Cilk after a break? stack_low_watermark?
  void print_states() = delete; // FILE *output = stdout

  static inline size_t num_races() { return g_num_races; }
  static inline void enable_checking() { g_checking_disabled = false; }
  static inline void disable_checking() { g_checking_disabled = true; }
  static inline bool should_check() { return !g_checking_disabled; }

  // @todo{ Read environment variables for setting race-reporting policy.}
  static void set_policy(enum rd_policy p); // not thread-safe
  static void report_race(void* addr);
  static void check_access(bool is_read, void* rip, void* addr, size_t mem_size);
}; // struct race_detector
