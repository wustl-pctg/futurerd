#include "reach_nonblock.hpp"

// XXX: Make initial node, which must be attached.

using namespace spbags;

namespace reach {

/********** Parallelism Creation **********/
// Outgoing non-SP edge
void at_future_create(sframe_data *f) {
  // Basically spawn without worrying about sync nodes
  structured::at_future_create(&f->sp);

  node *u = t_current;
  u->attach();

  node *w = new node(); // left (executed next)
  node *v = new node(); // right

  w->id = m_R.add_node();
  m_R.add_edge(find(u)->id, w->id);
  v->id = m_R.add_node();
  m_R.add_edge(find(u)->id, w->id);

  f->last_cont = v;
  t_current = w;
}

void at_spawn(sframe_data *f) {
  structured::at_spawn(&f->sp);

  node *u = t_current;
  node *w = new node(); // left
  node *v = new node(); // right

  f->nodes.push_fork(u, w, v);

  // XXX: What if u is already attached??
  // Wouldn't we need something like this:
  // t_current->att_pred = (find(f)->attached()) ? f : find(f)->att_pred;
  v->add_pred = find(u)->att_pred;
  w->add_pred = find(u)->att_pred;

  f->last_cont = v;
  t_current = w;
}

/********** Parallelism Deletion **********/
// This won't be called until the future has actually finished
void at_future_get(sframe_data *f, sfut_data *fut) {
  assert(fut->put_strand);

  node *u = t_current;
  node *v = new node(m_R.add_node()); // make attached
  node *w = fut->put_strand;

  m_R.add_edge(find(u)->id, v->id);

  assert(find(w)->attached());
  m_R.add_edge(find(w)->id, v->id);

  t_current = v;
}

// Returns the new node j
node* binary_join(node *f, // fork node
                  node *lfc, node *rfc, // left, right fork children
                  node *ljp, node *rjp // left, right join children
                  ) {
  node *j = new node(); // join node

  // No non-SP edges
  if (!find(p)->attached() && !find(p)->attached()) {
    f->merge(ljp);
    f->merge(rjp);
    f->merge(j);

    // Both ides incident on non-SP edges
  } else if (find(p)->attached() && find(p)->attached()) {
    f->attach();
    m_R.add_edge(find(f)->id, find(c)->id);
    m_R.add_edge(find(f)->id, find(c)->id);

    j->id = m_R.add_node();
    m_R.add_edge(find(p)->id, j->id);
    m_R.add_edge(find(p)->id, j->id);

  } else {
    node *att_pred, *unatt_pred; // attached/unattached join parents
    node *att_succ, *unatt_succ; // attached/unattached fork children
    if (find(p)->attached()) {
      att_pred = find(p); att_succ = find(c);
      unatt_pred = find(p); unatt_pred = find(c);
    } else {
      att_pred = find(p); att_succ = find(c);
      unatt_pred = find(p); unatt_pred = find(c);
    }
    assert(att_pred->attached() && att_succ->attached());
    assert(!unatt_pred->attached() && !unatt_succ->attached());

    if (!find(f)->attached()) f->merge(att_succ);
    att_pred->merge(j);
    unatt_find(d)->att_succ = find(j);
  }

  return j;
}

void at_sync(sframe_data *frame) {
  
  node *f; // fork 
  node *s1; // left fork child (s_1)
  node *s2; // right fork child (s_2)
  node *t1; // left join parent (t_1)
  node *t2 = t_current; // right join parent (t_2)

  
  while (frame->nodes.pop(&f, &s1, &s2, &t1)) {
    t2 = binary_join(f, s1, s2, t1, t2);
  }

  // Set current node
  t_current = t2;
}

/********** Continuations **********/
// Returning from a spawned function or future function.
// This is all the other non-join nodes, right?
void continuation(sframe_data *f) {
  t_current = f->cont;
  f->cont = nullptr;
}

void at_spawn_continuation(sframe_data *f, sframe_data *p) {
    structured::at_continuation(&f->sp);
    continuation(f);
}

void at_future_continuation(sframe_data *f, sframe_data *p) {
  continuation(f);
}

/********** Cilk/future task functions **********/
void at_cilk_function_start(sframe_data *f, sframe_data *p) {
  structured::at_cilk_function_start(&f->sp, &p->sp);
}
void at_cilk_function_end(sframe_data *f, sframe_data *p) {
  structured::at_cilk_function_start(&f->sp, &p->sp);
}

// Called when a future task completes. We know we're going to have
// non-SP edges out from here, we just don't know where to yet.
void at_future_finish(sfut_data *fut) {
  // Save for later, i.e. at_get
  fut->put_strand = t_current;

  // NB: Is it okay to do this here? Seems easier than during
  // at_get(), since a future may have get() called on it multiple
  // times.
  // t_current->attach();
}



} // namespace reach
