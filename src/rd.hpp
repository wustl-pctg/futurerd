// Race detection class and public API
#pragma once

#include "rd.h"

#include "reach.hpp"
#include "shadow_mem.hpp"
#include "shadow_stack.hpp"

// On the one hand, we only ever want one race detector anyway, so it
// seems silly to use a class with all static data. On the other hand,
// this makes initialization slightly easier when using a static
// library.
class race_detector {
public:
  static shadow_stack<sframe_data> t_sstack;
  static uint64_t t_stack_low_watermark;
  static bool t_clear_stack;
  static shadow_mem g_smem;
  static reach_ds g_reach;

  static enum rd_policy g_policy;
  static size_t g_num_races;
  static int check_disabled;

  race_detector();
  ~race_detector();
  
  //void reset() = delete; // re-enter Cilk after a break? stack_low_watermark?
  //void print_state() = delete; // FILE *output = stdout

  static inline size_t num_races() { return g_num_races; }
  //static inline void enable_checking() { t_checking_disabled = false; }
  //static inline void disable_checking() { t_checking_disabled = true; }
  static void enable_checking();
  static void disable_checking();
  static inline bool should_check() { return check_disabled == 0; }

  // @todo{ Read environment variables for setting race-reporting policy.}
  static void set_policy(enum rd_policy p); // not thread-safe
  static void report_race(void* addr, uint64_t last_rip, uint64_t this_rip);
  static void check_access(bool is_read, void* rip, void* addr, size_t mem_size);
  static smem_data* active();
}; // struct race_detector
