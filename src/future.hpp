#include <cassert>

/// @todo{Ideally the race detection functions in the future class
/// will be split out like the other cilk tool functions.}
#ifdef RACE_DETECT
#include "futurerd.hpp"
#else
class rd_info {
  at_create() {}
  at_finish() {}
  at_get() {}
};
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
  rd_info m_rd_info;

public:

  future() : m_stat(status::STARTED) {}
  void finish(T val) { m_value = val; m_stat = status::DONE; m_rd_info.at_finish(); }
  T get() { assert(m_stat == status::DONE); m_rd_info.at_get(); return m_value; }
}; // class future

template<typename T>
class structured_future {
private:
  enum class status { STARTED, DONE, TOUCHED };

  status m_stat;
  T m_value;

  // If we want to verify that this is structured at runtime, we can
  // keep an SP node here (english & hebrew) telling us where we
  // created this future. Then make sure that always precedes the
  // touch point.
  // sp_node m_create_point;

public:
  structured_future() : m_stat(status::STARTED) {}
  void finish(T val) { m_value = val; m_stat = status::DONE; }
  T get() { assert(m_stat == status::DONE); m_stat = status::TOUCHED; return m_value; }

}; // class structured_future


} // namespace cilk
