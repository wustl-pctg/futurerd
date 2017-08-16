// Should be a reachability data structure ONLY
#include "spbag.hpp"

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
  void at_future_finish(sframe_data *f, sframe_data *p, sfut_data *fut);

private:
  // Helper function for parallelism creation (spawns + create_future)
  void new_function(sframe_data *f);
  // Helper function for continuations
  void continuation(sframe_data *f, sframe_data *p);

}; // class structured
} // namespace reach
