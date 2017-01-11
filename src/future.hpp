#include <cassert>

namespace cilk {

template<typename T>
class pg_future { // put/get future
private:
  // Do we need the separate pg_status with INVALID
  enum pg_status { INVALID, STARTED, DONE };
  pg_status status;
  T value;

#ifdef RACEDETECT
  futurerd::strand_t put_strand;
#endif
  
public:

  pg_future() : status(INVALID) {}
  void start() { status = STARTED; } // Do we need this?
  void put(T val)
  {
#ifdef RACE_DETECT
    put_strand = futurerd::nontree_outgoing();
#endif
    value = val;
    
    // @TODO{Parallel: Mem fence in future::put}
    status = DONE;
  }
  
  T get()
  {
#ifdef RACE_DETECT
    futurerd::nontree_incoming(put_strand);
#endif
    // For sequential version
    assert(status == DONE);

    // For parallel version
    //if (status != DONE) cilk_suspend();
    // while (status != DONE)
    //   ;

    return value;
  }
};

// async futures
//enum class async_status { STARTED, DONE };

// For now, put/get futures only
//typedef pg_future future;
template<typename T> using future = pg_future<T>;


} // namespace cilk
