#include <cassert>

/// @todo{Ideally the race detection functions in the future class
/// will be split out like the other cilk tool functions.}
#define RACE_DETECT
#ifdef RACE_DETECT
#include "futurerd.hpp"
#else
namespace futurerd {
struct futurerd_info {};
void at_create() {}
void at_finish() {}
void at_get() {}
} // namespace futurerd
#endif

// A hack since I don't want to change the compiler.
// Asynchronously start foo(x, y) with
// create_future(type, name, foo, x, y)
// expanding to (in the sequential case)
// future<type> name = future<type>(); name.finish(foo(x,y));
// If we ever do this in parallel we'll need to do something like a
// detach.
#define create_future(T,f,func,args...) \
  cilk::future<T> f; f.finish(func(args))

// A future that doesn't automatically finish
// Requires the programmer to use f.finish(...) in the function
#define create_future2(T,f,func,args...) \
  cilk::future<T> f; func(args)

namespace cilk {

template<typename T>
class future {
private:
  enum class status { STARTED, DONE };

  status m_stat;
  T m_value;
  futurerd::future_rd_info m_rd_info;

public:

  future() : m_stat(status::STARTED) { futurerd::at_create(&m_rd_info); }
  void finish(T val) {
    m_value = val;
    m_stat = status::DONE;
    futurerd::at_finish(&m_rd_info);
  }
  T get() {
    assert(m_stat == status::DONE);
    futurerd::at_get(&m_rd_info);
    return m_value;
  }
}; // class future

} // namespace cilk
