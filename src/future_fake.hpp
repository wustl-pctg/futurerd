// Hacks to create futures since we don't want to change the compiler
// (yet). Note that this doesn't actually create any parallelism yet,
// so you can only run sequentially.
#include "../cilkrts/include/internal/cilk_fake.h"

extern "C" {
void cilk_enter_begin();
void cilk_enter_end(__cilk_fake_stack_frame *sf, void *rsp);
}

// Since the compiler doesn't recognize create_future calls, you'll
// need to call FUTURE_PROLOG at the beginning of a function that
// calls create_future, and FUTURE_EPILOG at the end. You don't need
// to do this if the function also uses the 'spawn' keyword.
#define FUTURE_PROLOG()                         \
  CILK_FAKE_PROLOG();                           \
  cilk_enter_begin();                           \
  CILK_FAKE_DEFERRED_ENTER_FRAME(__cilk_sf);     \
  cilk_enter_end(&__cilk_sf, NULL)

#define FUTURE_EPILOG() CILK_FAKE_EPILOG()

/********** Launching futures **********/
// #define create_future(T,f,func,args...)      \
//   futurerd::disable_checking();              \
//   cilk::future<T> f;                         \
//   futurerd::enable_checking();               \
//   f.finish(func(args))

// Convenience
#define __DC futurerd::disable_checking()
#define __EC futurerd::enable_checking()

/** Asynchronously start foo(x,y) -> int with
 *  cilk_async(int, f, foo, x, y)
 */
#define cilk_async(T,name,func,args...)         \
  __DC; auto (name) = new cilk::future<T>(); __EC;  \
  (name)->finish(func(args));

// Allocate new future, but set it to a given variable
#define create_future(T,fut,func,args...)       \
  __DC; (fut) = new cilk::future<T>(); __EC;    \
  fut->finish(func(args));

// Use preallocated memory for the future itself
#define reuse_future(T,loc,func,args...)        \
  __DC; new(loc) cilk::future<T>(); __EC;       \
  loc->finish(func(args));

// When we need to return a pair of futures...
// can't think of a better way to do this...
#define cilk_pg_async2(T,f1,f2,func,args...)       \
  __DC; auto (f1) = new cilk::future<T>();            \
  auto (f2) = new cilk::future<T>(); __EC;            \
  func(args);                                   \
  f1->finish(); f2->finish();                   \

/** Versions that manually call put(), possibly before the end of the
    function.
 */
#define cilk_pg_async(T,name,func,args...)      \
  __DC; auto (name) = new cilk::future<T>(); __EC;  \
  func(args);                                   \
  (name)->finish();
#define create_pg_future(T,fut,func,args...)    \
  __DC; (fut) = new cilk::future<T>(); __EC;    \
  func(args);                                   \
  (fut)->finish();
#define reuse_pg_future(T,loc,func,args...)     \
  __DC; new(loc) cilk::future<T>(); __EC;       \
  func(args);                                   \
  loc->finish();

/** Verisons that automatically deduce the future type. */
#define FTYPE(func,args...) decltype((func)(args))
#define cilk_auto_async(name,func,args...)      \
  cilk_async(FTYPE(func,args),name,func,args)
#define create_auto_future(fut,func,args...)    \
  create_future(FTYPE(func,args),fut,func,args)
#define reuse_auto_future(loc,func,args...)     \
  reuse_future(FTYPE(func,args),loc,func,args)
#define cilk_auto_pg_async(name,func,args...)     \
  cilk_pg_async(FTYPE(func,args),name,func,args)
#define create_auto_pg_future(fut,func,args...)     \
  create_pg_future(FTYPE(func,args),fut,func,args)
#define reuse_auto_pg_future(loc,func,args...)    \
  reuse_pg_future(FTYPE(func,args),loc,func,args)

// Some details on how the cilk fake macros implement spawn. Will be
// useful when implementing parallel futures.
/** 
CILK_FAKE_SPAWN_R(a, fib(n-1)) -> a = spawn fib(n-1);
 CILK_FAKE_CALL_SPAWN_HELPER(CILK_FAKE_SPAWN_HELPER(expr, args), sf);
 
CILK_FAKE_CALL_SPAWN_HELPER:
  CILK_FAKE_DEFERRED_ENTER_FRAME(sf) --> if (!sf.worker) __cilk_fake_enter_frame(sf)
  CILK_FAKE_SAVE_FP(sf) --> assembly code (for non-Windows)
  if (__builtin_expect(! CILK_SETJMP(sf.ctx), 1)) {
    helper();
  }


CILK_FAKE_SPAWN_HELPER creates a lambda function which calls
CILK_FAKE_SPAWN_HELPER_BODY, which:
  CILK_FAKE_SPAWN_HELPER_PROLOG(sf);
  expand args
  fake_detach
  expression (function call)
  CILK_FAKE_SPAWN_HELPER_EPILOG

 */
