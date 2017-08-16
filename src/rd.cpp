#include "rd.hpp"
#include "debug.hpp"

// XXX: Compiler doesn't define __cilk?
#define USE_CILK_API
#include <cilk/cilk_api.h>
// #include <internal/abi.h>

uint64_t race_detector::t_stack_low_watermark = (uint64_t)(-1);
bool race_detector::t_clear_stack = false;
enum rd_policy race_detector::g_policy = RD_CONTINUE;
size_t race_detector::g_num_races = 0;
bool race_detector::t_checking_disabled = false;
//reach::structured race_detector::g_reach;
shadow_mem race_detector::g_smem;

race_detector::race_detector() {
  // Ensure only one race detector
  static bool init = false;
  assert(init == false);
  init = true;
  t_sstack.push();

  __cilkrts_set_param("nworkers", "1");
}

race_detector::~race_detector() {}

void race_detector::set_policy(enum rd_policy p) {
  if (p >= RD_LAST_POLICY)
    debug::die("Invalid Detection Policy\n");
  g_policy = p;
}

// @todo{report_race() should report what kind of race, i.e. RW, WR, or WW}
void race_detector::report_race(void* addr) {
  if (! (g_policy == RD_SILENT) )
    fprintf(stderr, "Race detected at %p\n", addr);
  g_num_races++;
  if (g_policy == RD_ABORT)
    std::abort();
}

void race_detector::check_access(bool is_read, void* rip,
                                 void* addr, size_t mem_size) {
  shadow_mem::addr_info_t *slot = g_smem.find((uint64_t)addr);

  // no previous accesses
  if (slot == nullptr) {
    // XXX: Fix Null: should be active()
    g_smem.insert(is_read, (uint64_t)addr, nullptr, (uint64_t)rip);
    return;
  }
  
  // check race with last writer
  if (slot->last_writer.access != nullptr // if last writer exists
      && !slot->last_writer.access->precedes_now())
    report_race(addr);
  
  // if write, check race with last read
  if (!is_read // if a write
      && slot->last_reader.access != nullptr // and last reader exists
      && !slot->last_reader.access->precedes_now()) {
    report_race(addr);
  }
  
  // update shadow mem
  // XXX: again, fix nullptr
  g_smem.update(slot, is_read, (uint64_t)addr, nullptr, (uint64_t)rip);
}
