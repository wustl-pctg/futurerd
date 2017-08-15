#include <cassert>

/// @todo{Ideally the race detection functions in the future class
/// will be split out like the other cilk tool functions.}
//#define RACE_DETECT
#ifdef RACE_DETECT
#include "futurerd.hpp"

// Not supported in our version of clang...
//#define NOSANITIZE __attribute__((no_sanitizer("thread")))
#define NOSANITIZE
#else
__attribute__((weak)) void cilk_future_create() {}
__attribute__((weak)) void cilk_future_get(sfut_data *) {}
__attribute__((weak)) void cilk_future_finish() {}
__attribute__((weak)) void cilk_future_put() {}
namespace futurerd {
struct futurerd_info {};
__attribute__((weak)) void at_put(struct futurerd_info*) {}
} // namespace futurerd
#define NOSANITIZE
#endif

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
  futurerd::futurerd_info m_rd_info;

public:

  NOSANITIZE future() : m_stat(status::CREATED)
  { cilk_future_create(); }
  
  NOSANITIZE void put(T val) {
    m_val = val;
    m_stat = status::PUT;
    futurerd::at_put(&m_rd_info);
  }

  NOSANITIZE void finish() {
    assert(m_stat == status::PUT);
    m_stat = status::DONE;
    cilk_future_finish(&m_rd_info);
    // For sequential futures it's fine to call this here
  }

  NOSANITIZE void finish(T val) {
    put(val);
    finish();
  }

  NOSANITIZE bool ready() { return m_stat >= status::PUT; }
  NOSANITIZE T get() {

    // True for our sequential version
    assert(m_stat == status::DONE);

    while (m_stat < status::PUT)
      /* cilk_yield() */ ;
    //m_got_count++;
    cilk_future_get(&m_rd_info);

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
