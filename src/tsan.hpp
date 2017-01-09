#include <cassert>

extern "C" {

extern __thread int t_checking_disabled;

#define INLINE __attribute__((always_inline))
//[[gnu::always_inline]]

inline extern void enable_checking() {
  t_checking_disabled--;
  assert(t_checking_disabled >= 0);
}


inline extern void disable_checking() {
  assert(t_checking_disabled >= 0);
  t_checking_disabled++;
}

} // extern "C"
