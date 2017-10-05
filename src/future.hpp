#include <cassert>

/// @todo{Ideally the race detection functions in the future class
/// will be split out like the other cilk tool functions.}
//#define RACE_DETECT
#ifdef RACE_DETECT
#include "rd.hpp"

extern "C" {
void cilk_future_create();
void cilk_future_get_begin(sfut_data *);
void cilk_future_get_end(sfut_data *);
void cilk_future_finish_begin(sfut_data *);
void cilk_future_finish_end(sfut_data *);
void cilk_future_put_begin(sfut_data *);
void cilk_future_put_end(sfut_data *);
}

// Not supported in our version of clang...
//#define NOSANITIZE __attribute__((no_sanitizer("thread")))
#define NOSANITIZE
#else
/// @todo{ Put weak symbols for cilktool functions for futures in the runtime }
struct sfut_data {};
__attribute__((weak)) void cilk_future_create() {}
__attribute__((weak)) void cilk_future_get_begin(struct sfut_data *) {}
__attribute__((weak)) void cilk_future_get_end(struct sfut_data *) {}
__attribute__((weak)) void cilk_future_finish_begin(struct sfut_data *) {}
__attribute__((weak)) void cilk_future_finish_end(struct sfut_data *) {}
__attribute__((weak)) void cilk_future_put_begin(struct sfut_data *) {}
__attribute__((weak)) void cilk_future_put_end(struct sfut_data *) {}
#define NOSANITIZE
#endif // RACE_DETECT

#ifndef __cilkfutures
#include "future_fake.hpp"
#endif

namespace cilk {

template<typename T>
class future {
private:
  enum class status { CREATED, // memory allocated, initialized
                      PUT, // value is ready
                      DONE, // strand has finished execution
  };

  status m_stat;
  T m_val;
  sfut_data m_rd_data;

public:

  NOSANITIZE future() : m_stat(status::CREATED)
  { cilk_future_create(); }
  
  NOSANITIZE void put(T val) {
    cilk_future_put_begin(&m_rd_data);
    m_val = val;
    m_stat = status::PUT;
    cilk_future_put_end(&m_rd_data);
  }

  NOSANITIZE void finish() {
    cilk_future_finish_begin(&m_rd_data);
    assert(m_stat == status::PUT);
    m_stat = status::DONE;
    cilk_future_finish_end(&m_rd_data);
  }

  NOSANITIZE void finish(T val) {
    // Have to do this b/c NOSANITIZE doesn't work for old clang version...
    race_detector::disable_checking();
    put(val); finish();
    race_detector::enable_checking();
  }
  
  NOSANITIZE bool ready() { return m_stat >= status::PUT; }
  NOSANITIZE T get() {
    race_detector::disable_checking();
    cilk_future_get_begin(&m_rd_data);
    assert(m_stat == status::DONE); // for sequential futures
    cilk_future_get_end(&m_rd_data);
    race_detector::enable_checking();
    return m_val;
  }
}; // class future

// template<typename T>
// class future_ptr {
// public:
//   future<T>* m_fut;
//   future_ptr() = delete;
//   future_ptr(future<T>* f) : m_fut(f) {}
//   bool ready() { return (m_fut) ? m_fut->ready() : false; }
//   void touch() { assert(m_fut); m_fut->get(); }

//   bool operator==(const void* p) const { return m_fut == p; }


//   // Convenience, allows us to use futures in place of variables everywhere
//   // Do we really want to call get here? It may increase get_count
//   // even if we know it's already ready...
//   T* operator->() const { return m_fut->get(); }
//   T* operator*() const { return m_fut->get(); }
// }; // class future_ptr


} // namespace cilk
