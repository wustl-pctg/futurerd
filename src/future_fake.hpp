/********** Launching futures **********/

#define FUTURE_PROLOG()
#define FUTURE_EPILOG()

// Convenience
#define __DC //disable_checking()
#define __EC //enable_checking()
#define __FS //cilk_future_start()
#define __FC //cilk_future_continuation();

/** Asynchronously start foo(x,y) -> int with
 *  cilk_async(int, f, foo, x, y)
 */
#define cilk_async(T,name,func,args...)         \
  __DC; auto (name) = new cilk::future<T>(); __EC;  \
  __FS; (name)->finish(func(args)); __FC;

// Allocate new future, but set it to a given variable
#define create_future(T,fut,func,args...)       \
  __DC; (fut) = new cilk::future<T>(); __EC;    \
  __FS; fut->finish(func(args)); __FC;

// Use preallocated memory for the future itself
#define reuse_future(T,loc,func,args...)        \
  __DC; new(loc) cilk::future<T>(); __EC;       \
  __FS; loc->finish(func(args)); __FC;

// When we need to return a pair of futures...
// can't think of a better way to do this...
#define cilk_pg_async2(T,f1,f2,func,args...)       \
  __DC; auto (f1) = new cilk::future<T>();            \
  auto (f2) = new cilk::future<T>(); __EC;            \
  __FS; func(args);                                    \
  f1->finish(); f2->finish(); __FC;                    \

/** Versions that manually call put(), possibly before the end of the
    function.
 */
#define cilk_pg_async(T,name,func,args...)      \
  __DC; auto (name) = new cilk::future<T>(); __EC;  \
  __FS; func(args);                                  \
  (name)->finish(); __FC;
#define create_pg_future(T,fut,func,args...)    \
  __DC; (fut) = new cilk::future<T>(); __EC;    \
  __FS; func(args);                              \
  (fut)->finish(); __FC;
#define reuse_pg_future(T,loc,func,args...)     \
  __DC; new(loc) cilk::future<T>(); __EC;       \
  __FS; func(args);                              \
  loc->finish(); __FC;

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
