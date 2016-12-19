namespace cilk {

// Do we need the separate pg_status with INVALID
typedef enum { INVALID, STARTED, DONE } pg_status;
typedef enum { STARTED, DONE } async_status;

  // For now, put/get futures only
  typedef pg_future future;

template<type T>
class pg_future { // put/get future
private:
  pg_status status;
  T value;
  //strand_t put_strand;
public:

  pg_future() : status(INVALID) {}
  void start() { status = STARTED; } // Do we need this?
  void put(T val)
  {
    // {
    //   merge(t_strand, S);
    //   merge(t_strand, C);

    //   strand_t next = new_set();
    //   S = new_set();
    //   add_edge(C, next);
    //   C = next;
    // }
    
    value = val;
    //put_strand = t_strand;
    
    // @TODO{Parallel: Mem fence in future::put}
    status = DONE;
  }
  
  T get()
  {
    assert(status == DONE);
    // {
    //   strand_t next = new_set();
    //   add_edge(C, next);
    //   add_edge(find(put_strand), next);
    //   C = next;
    // }
    
    //if (status != DONE) cilk_suspend();
    // while (status != DONE)
    //   ;

    return value;
  }
};

} // namespace cilk
