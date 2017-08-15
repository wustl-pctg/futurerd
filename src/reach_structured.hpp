// Should be a reachability data structure ONLY
#include "sbpag.hpp"

// XXX: move this
// This is just a template that the real reachability data structures
// must follow. You could probably mess around with virtual functions
// and the like (although I don't know about requiring the definition
// of an sframe_data struct). But (1) I'm not sure that's any clearer,
// and (2) the virtual functions may add overhead if the program uses
// enough parallel operations.

namespace reach {
class structured {
public:

  struct sframe_data {
    spbag *Sbag, *Pbag;
  };

  struct smem_data {
    spbag *last_reader;
    spbag *last_writer;
  };

  struct sfut_data {
    spbag *put_strand;
  };

  structured() = delete;
  structured(sframe_data *initial);
  //~structured();

  // Parallelism creation
  void at_future_create(sframe_data *f);
  void at_spawn(sframe_data *f);

  // Parallelism deletion
  // void at_future_put(sfut_data*);
  void at_future_get(sframe_data *f, sfut_data*);
  void at_sync(sframe_data *f);

  // Continuations
  void at_spawn_continuation(sframe_data *f, sframe_data *p);
  void at_future_finish(sfut_data*);

  // Cilk functions
  // void at_cilk_function_start(sframe_data *f, sframe_data *parent);
  // void at_cilk_function_end(sframe_data *f, sframe_data *p);

private:
  // Helper function for parallelism creation (spawns + create_future)
  void new_function(sframe_data *f);
  // Helper function for continuations
  void continuation(sframe_data *f, sframe_data *p);

}; // class structured
} // namespace reach
