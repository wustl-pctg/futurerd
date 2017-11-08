// Should be a reachability data structure ONLY

// We don't actually need the structured part, just the fork/join part
// of SP Bags.
#include "reach_structured.hpp"
#include "reach_gen.hpp"

namespace reach {
class nonblock {
public:
  struct node : utils::uf::node { // inherit from union-find node type

    node *att_pred = nullptr;
    node *att_succ = nullptr;
    reach::general::node id; // id of node in R graph, or 0 if not in R (size_t)
    structured::smem_data *sbag; // ANGE: a bit hacky, but good enough for now
    bool attached() { return id > 0; }
    node* find() { return static_cast<node*>(utils::uf::find(this)); }
    bool precedes_now();
    node(reach::general::node _id = 0) : id(_id) {}
    void merge(node *n) { utils::uf::merge(this, n); }
  }; // struct node

  struct sframe_data {
    structured::sframe_data sp;

    // Future RD stuff.
    node *fork = nullptr;
    node *lfc = nullptr;
    node *rfc = nullptr;
    node *ljp = nullptr; // rjp is t_current at sync

    node *future_fork = nullptr;

    // continuation node, created and saved at spawns and create_futures
    //node *cont = nullptr; // XXX: Update to use of node_stack?

    // NB: We will probably need to save some more things here to handle
    // joins with in-degree > 2.
    //node_stack nodes;
  };

  //struct smem_data {};
  using smem_data = node;

  struct sfut_data {
    node* put_strand;
  };

  // Member data
public:
  reach::general m_R;
  static reach::general* s_R;
  reach::structured m_sp; // series-parallel reachability
  static node* t_current; // XXX: necessary?

public:
  nonblock() {}
  nonblock(sframe_data *initial);
  void init(sframe_data *initial);

  smem_data* active(sframe_data *f);

  // Parallelism creation
  void at_future_create(sframe_data *f);
  void at_spawn(sframe_data *f, sframe_data *helper);

  // Parallelism deletion
  // void at_future_put(sfut_data*);
  void at_future_get(sframe_data *f, sfut_data* fut);
  void at_sync(sframe_data *f);

  // Continuations
  void at_spawn_continuation(sframe_data *f, sframe_data *p);
  void at_future_continuation(sframe_data *f, sframe_data *p);

  // Cilk/future task functions
  void at_future_finish(sframe_data *f, sframe_data *p, sfut_data* fut);

  void begin_strand(sframe_data *f, sframe_data *p);
  bool precedes_now(sframe_data *f, smem_data *last_access); 

private:
  // Helper function for parallelism creation (spawns + create_future)
  // void create_strand(sframe_data *f);

  // Helper function for continuations
  void continuation(sframe_data *f);

  // make the set containing u attached if not already
  void attachify(node *n);

  // Helper function for syncs
  node* binary_join(node *f, // fork node
                    node *lfc, node *rfc, // left, right fork children
                    node *ljp, node *rjp); // left, right join children


  static inline void 
  copy_nonblock_data(sframe_data *dst, sframe_data *src) {
    dst->fork = src->fork;  
    dst->lfc = src->lfc;  
    dst->rfc = src->rfc;  
    dst->ljp = src->ljp;  
    dst->future_fork = src->future_fork;  
  }

}; // class nonblock
} // namespace reach
