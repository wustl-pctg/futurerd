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
namespace futurerd {
struct futurerd_info {};
__attribute__((weak)) void at_create(struct futurerd_info*) {}
__attribute__((weak)) void at_finish(struct futurerd_info*) {}
__attribute__((weak)) void at_get(struct futurerd_info*) {}
__attribute__((weak)) void at_put(struct futurerd_info*) {}
__attribute__((weak)) void enable_checking() {}
__attribute__((weak)) void disable_checking() {}
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
                      //STARTED, // strand has started execution
                      PUT, // value is ready
                      DONE, // strand has finished execution
  };

  status m_stat;
  T m_val;
  //size_t m_got_count; // # times get() has been called
  futurerd::futurerd_info m_rd_info;

public:

  NOSANITIZE future() : m_stat(status::CREATED)
  { futurerd::at_create(&m_rd_info); }

  // We don't allow futures to be created and then started later
  // NOSANITIZE void start() {
  //   assert(m_stat == status::CREATED);
  //   m_stat = status::STARTED;
  // }
  
  NOSANITIZE void put(T val) {
    m_val = val;
    m_stat = status::PUT;
    futurerd::at_put(&m_rd_info);
  }

  NOSANITIZE void finish() {
    assert(m_stat == status::PUT);
    m_stat = status::DONE;
    futurerd::at_finish(&m_rd_info);
  }

  NOSANITIZE void finish(T val) {
    //futurerd::disable_checking(); // disable here or in at_finish()?
    put(val);
    finish();
    //futurerd::enable_checking();
  }



  NOSANITIZE bool ready() { return m_stat >= status::PUT; }
  NOSANITIZE T get() {
    //futurerd::disable_checking(); // disable here or in at_get()?

    // True for our sequential version
    assert(m_stat == status::DONE);

    while (m_stat < status::PUT)
      /* cilk_yield() */ ;
    //m_got_count++;
    futurerd::at_get(&m_rd_info);
    //futurerd::enable_checking();
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
