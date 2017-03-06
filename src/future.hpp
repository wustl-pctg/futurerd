#include <cassert>

/// @todo{Ideally the race detection functions in the future class
/// will be split out like the other cilk tool functions.}
#ifdef RACE_DETECT
#define RD(func) rd_ ## func
#else
#define RD(func)
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

  // Race detection stuff
#ifdef RACE_DETECT

  void rd_create_future() {}
  void rd_finish_future() {}
  void rd_get_future() {}
#endif

public:

  future() : m_stat(status::STARTED) { RD(create_future(this)); }
  void finish(T val) { m_value = val; m_stat = status::DONE; RD(finish_future(this)); }
  T get() { assert(m_stat == status::DONE); return m_value; }
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
