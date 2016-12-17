//namespace cilk {

// Do we need the separate pg_status with INVALID
typedef enum { INVALID, STARTED, DONE } pg_status;
typedef enum { STARTED, DONE } async_status;


template<type T>
class pg_future { // put/get future
private:
  pg_status status;
  T value;
  strand_t put_strand;
public:

  pg_future() : status(INVALID) {}
  void start() { status = STARTED; } // Do we need this?
  void put(T val)
  {
    {
      merge(t_strand, S);
      merge(t_strand, C);

      strand_t next = new_set();
      S = new_set();
      add_edge(C, next);
      C = next;
    }
    
    value = val;
    put_strand = t_strand;
    // Mem fence
    status = DONE;
  }
  T get()
  {
    assert(status == DONE);
    {
      strand_t next = new_set();
      add_edge(C, next);

      // Is the find right?
      add_edge(find(put_strand), next);

      C = next;
    }
    
    //if (status != DONE) cilk_suspend();
    while (status != DONE)
      ;

    return value;
  }
};

template<type T>
class async_future { // async future
private:
  async_status status;
  T value;
  strand_t put_strand;

  void finish(T val) // called implicitly (by the runtime)
  {
    value = val;
    put_strand = t_strand;
    // Mem fence
    status = DONE;
  }
public:
  async_future() : status(STARTED) {}
  T get()
  {
    //if (status != DONE) cilk_suspend();
    while (status != DONE)
      ;

    return value;
  }
  
};
