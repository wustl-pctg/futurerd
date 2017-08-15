#include "rd.hpp"
#include "debug.hpp"

// #define USE_CILK_API
// #include <internal/abi.h>
// #include <cilk/cilk_api.h>


void race_detector::race_detector() {
  // Ensure only one race detector
  static bool init = false;
  assert(init == false);
  init = true;

  // Obviously this will need to change when we do this in parallel.
  t_stack_low_watermark = (uint64_t)(-1);
  t_sstack.push();

  __cilkrts_set_param("nworkers", "1");
}

void race_detector::~race_detector() {}

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
    g_smem.insert(is_read, (uint64_t)addr, active(), (uint64_t)rip);
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
  g_smem.update(slot, is_read, (uint64_t)addr, active(), (uint64_t)rip);
}
