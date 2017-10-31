// Should be a reachability data structure ONLY
#pragma once

#include "spbag.hpp"

namespace reach {
class structured {
public:

  struct sframe_data {
    spbag *Sbag, *Pbag;
  };

  using smem_data = spbag;

  struct sfut_data {
    spbag *put_strand;
  };

  structured() {}
  //structured() = delete;
  structured(sframe_data *initial);
  void init(sframe_data *initial);
  //~structured();
  smem_data* active(sframe_data *f);

  // Parallelism creation
  void at_future_create(sframe_data *f);
  void at_spawn(sframe_data *f, sframe_data *helper);

  // Parallelism deletion
  // void at_future_put(sfut_data*);
  void at_future_get(sframe_data *f, sfut_data*);
  void at_sync(sframe_data *f);

  // Continuations
  void at_spawn_continuation(sframe_data *f, sframe_data *p);
  void at_future_finish(sframe_data *f, sframe_data *p, sfut_data *fut);

  // Actually start a "strand" function that has already been "created"
  void begin_strand(sframe_data *f, sframe_data *p);
  // Helper function for parallelism creation (spawns + create_future)
  // Public for use by nonblock (should be friend class...)
  void create_strand(sframe_data *f);

private:
  // Helper function for continuations
  void continuation(sframe_data *f, sframe_data *p);

}; // class structured
} // namespace reach
