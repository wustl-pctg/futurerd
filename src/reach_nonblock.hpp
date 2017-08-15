// Should be a reachability data structure ONLY

// We don't actually need the structured part, just the fork/join part
// of SP Bags.
#include "reach_structured.hpp"

// XXX: move this
// This is just a template that the real reachability data structures
// must follow. You could probably mess around with virtual functions
// and the like (although I don't know about requiring the definition
// of an sframe_data struct). But (1) I'm not sure that's any clearer,
// and (2) the virtual functions may add overhead if the program uses
// enough parallel operations.

namespace reach {
class nonblock {
public:
  struct node : utils::uf::node { // inherit from union-find node type

    node *att_pred = nullptr;
    node *att_succ = nullptr;
    genreach::node id; // id of node in R graph, or 0 if not in R (size_t)
    bool attached() { return id > 0; }

    node(genreach::node _id = 0) : id(_id) {}
  
    // make the set containing u attached if not already ("attachify")
    void attach() {
      node *su = this->find();
      if (su->attached()) return;
    
      // su is unattached
      su->id = g_R.add_node(); // make attached with node in R
      g_R.add_edge(find(su->att_pred)->id, su->id);
    }
  }; // struct node
  
  // Helper data structure for shadow frames
  struct node_stack {
    using index_t = uint8_t;
    static constexpr index_t STACK_SIZE = 4;
    index_t index = 0;
    node* fork_nodes[STACK_SIZE];
    node* cont_nodes[STACK_SIZE]; // do we need this? Duplication with last_cont
    node* succ_nodes[STACK_SIZE];
    node* left_pred_nodes[STACK_SIZE];

    void push_fork(node *fork, node* succ, node* cont) {
      assert(index < STACK_SIZE);
      fork_nodes[index] = fork;
      succ_nodes[index] = succ;
      cont_nodes[index] = cont;

      // "finalize" the fork
      void push_succ(node *left_pred) { left_pred_nodes[index++] = left_pred; }
      node* get_last_cont() { return cont_nodes[index - 1]; }
  
      // returns remaining size
      index_t pop(node** fork, node** succ, node** cont, node** left_pred) {
        if (index == 0) return 0;
        index--;
        *fork = fork_nodes[index];
        *cont = cont_nodes[index];
        *succ = succ_nodes[index];
        *left_pred = left_pred_nodes[index];
        return index;
      }
    }
  }; // struct node_stack
  
  struct sframe_data {
    structured::sframe_data sp;

    // Future RD stuff.
    // continuation node, created and saved at spawns and create_futures
    node *last_cont; // XXX: Update to use of node_stack?

    // NB: We will probably need to save some more things here to handle
    // joins with in-degree > 2.
    node_stack nodes;
  };

  struct smem_data {
    //spbag *last_reader;
    //spbag *last_writer;
  };

  struct sfut_data {
    node* put_strand;
  };
    
  // Member data
private:
  genreach m_R;
  node* t_current; // XXX: necessary?

public:
  // Parallelism creation
  void at_future_create(sframe_data *f);
  void at_spawn(sframe_data *f);

  // Parallelism deletion
  // void at_future_put(sfut_data*);
  void at_future_get(sframe_data *f, sfut_data* fut);
  void at_sync(sframe_data *f);

  // Continuations
  void at_spawn_continuation(sframe_data *f, sframe_data *p);
  void at_future_continuation(sframe_data *f);

  // Cilk/future task functions
  void at_future_finish(sfut_data*);

private:
  // Helper function for continuations
  void continuation(sframe_data *f);

  // Helper function for syncs
  node* binary_join(node *f, // fork node
                    node *lfc, node *rfc, // left, right fork children
                    node *ljp, node *rjp); // left, right join children


}; // class nonblock
} // namespace reach
