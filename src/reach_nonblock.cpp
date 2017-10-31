#include "reach_nonblock.hpp"
#include <cassert>

namespace reach {

using node = nonblock::node;

nonblock::nonblock(sframe_data *initial) { init(initial); }

nonblock::smem_data* nonblock::active(sframe_data *f) {
  return t_current;
}

// Should be done in a constructor if possible
void nonblock::init(sframe_data *initial) {
  m_struct.init(&initial->sp);
  t_current = new node();
  t_current->att_pred = t_current;
  attachify(t_current);
}

void nonblock::attachify(node* n) {
  node *su = n->find();
  if (su->attached()) return;

  // su is unattached
  su->id = m_R.add_node(); // make attached with node in R
  assert(su->att_pred);
  m_R.add_edge(su->att_pred->find()->id, su->id);
}

/********** Parallelism Creation **********/

void nonblock::begin_strand(sframe_data *f, sframe_data *p) {
  m_struct.begin_strand(&f->sp,&p->sp);
  // Future stuff?
}

void nonblock::create_strand(sframe_data *f) {
  m_struct.create_strand(&f->sp);
}

void nonblock::at_spawn(sframe_data *f, sframe_data *h) {
  m_struct.at_spawn(&f->sp, &h->sp);

  node *u = t_current; // fork node
  h->fork = u;
  h->rfc = h->ljp = nullptr;

  // Make new unattached set containing v
  node *v = new node(); // left child
  h->lfc = v;
  v->att_pred = (u->find()->attached()) ? u : u->find()->att_pred;

  //h->nodes.push_fork(u, w, v);
  // v->att_pred = u->find()->att_pred;
  // w->att_pred = u->find()->att_pred;

  // XXX anything else for the other child? Set att_pred?
  // node *w = new node(); // right child
  // h->rfc = w;
  // h->cont = w;

  t_current = v;
}

// Outgoing non-SP edge
// We don't have helper frames for futures, so this is actually the
// new frame created for the new strand
void nonblock::at_future_create(sframe_data *f) {
  // Basically spawn without worrying about sync nodes
  m_struct.at_future_create(&f->sp);

  node *u = t_current;
  assert(f->future_fork == nullptr);
  f->future_fork = u;
  attachify(u);

  // Create a new attached set
  node *v = new node(); // left (executed next, since we use eager execution)
  v->id = m_R.add_node();
  m_R.add_edge(u->find()->id, v->id);

  //node *w = new node(); // right
  // w->id = m_R.add_node();
  // m_R.add_edge(u->find()->id, w->id);

  //f->cont = w;
  t_current = v;
}

/********** Parallelism Deletion **********/
// This won't be called until the future has actually finished
// Incoming non-SP edge
void nonblock::at_future_get(sframe_data *f, sfut_data *fut) {
  assert(fut->put_strand);

  node *u = t_current;

  // Make a new attached set
  node *v = new node(m_R.add_node());
  m_R.add_edge(u->find()->id, v->id);

  // XXX: Not in the pseudocode but I don't see how you couldn't have this.
  node *w = fut->put_strand;
  assert(w->find()->attached());
  m_R.add_edge(w->find()->id, v->id);

  t_current = v;
}

// Returns the new node j
node* nonblock::binary_join(node *f, // fork node
                            node *lfc, node *rfc, // left, right fork children
                            node *ljp, node *rjp // left, right join children
                            ) {
  node *j = new node(); // join node
  bool ljp_attached = ljp->find()->attached();
  bool rjp_attached = rjp->find()->attached();

  // No non-SP edges
  if (!ljp_attached && !rjp_attached) {
    f->merge(ljp);
    f->merge(rjp);
    f->merge(j);

    // Both sides incident on non-SP edges
  } else if (ljp_attached && rjp_attached) {
    //f->attach();
    attachify(f);
    m_R.add_edge(f->find()->id, lfc->find()->id);
    m_R.add_edge(f->find()->id, rfc->find()->id);

    j->id = m_R.add_node();
    m_R.add_edge(ljp->find()->id, j->id);
    m_R.add_edge(rjp->find()->id, j->id);

  } else {
    node *att_pred, *unatt_pred; // attached/unattached join parents
    node *att_succ, *unatt_succ; // attached/unattached fork children
    if (ljp_attached) {
      att_pred = ljp->find(); att_succ = lfc->find();
      unatt_pred = rjp->find(); unatt_succ = rfc->find();
    } else {
      att_pred = rjp->find(); att_succ = rfc->find();
      unatt_pred = ljp->find(); unatt_succ = lfc->find();
    }
    assert(att_pred->attached() && att_succ->attached());
    assert(!unatt_pred->attached() && !unatt_succ->attached());

    if (!f->find()->attached()) f->merge(att_succ);
    att_pred->merge(j);
    unatt_pred->find()->att_succ = j->find();
  }

  return j;
}

// Currently we only support binary join
void nonblock::at_sync(sframe_data *f) {

  //node *f; // fork
  // node *s1; // left fork child (s_1)
  // node *s2; // right fork child (s_2)
  // node *t1; // left join parent (t_1)
  //node *t2 = t_current; // right join parent (t_2)


  // while (frame->nodes.pop(&f, &s1, &s2, &t1)) {
  //   t2 = binary_join(f, s1, s2, t1, t2);
  // }

  // Set current node
  //t_current = t2;

  assert(f->fork); assert(f->lfc); assert(f->rfc); assert(f->ljp);
  t_current = binary_join(f->fork, f->lfc, f->rfc, f->ljp, t_current);
  f->fork = f->lfc = f->rfc = f->ljp = nullptr;
}

/********** Continuations **********/
// Returning from a spawned function or future function.
// This is all the other non-join nodes, right?
void nonblock::continuation(sframe_data *f) { t_current = new node(); }

// f is the helper frame, p is parent
void nonblock::at_spawn_continuation(sframe_data *f, sframe_data *p) {
  // Pop hasn't happened yet
  assert(f->fork); assert(f->lfc); assert(f->rfc == nullptr);
  m_struct.at_spawn_continuation(&f->sp, &p->sp);

  f->ljp = t_current;
  continuation(f);
  f->rfc = t_current;
  *p = *f;
}

void nonblock::at_future_continuation(sframe_data *f, sframe_data *p) {
  // Use p because the pop hasn't happened yet
  // Except we're not really doing anything with the "future helper"
  // frames...do we even need them?
  //continuation(p);
  assert(f->future_fork);
  node *w = new node(); // right
  w->id = m_R.add_node();
  m_R.add_edge(f->future_fork->find()->id, w->id);
  f->future_fork = nullptr;
  t_current = w;
}

// Called when a future task completes. We know we're going to have
// non-SP edges out from here, we just don't know where to yet.
void nonblock::at_future_finish(sframe_data *f, sframe_data *p, sfut_data* fut) {
  // Save for later, i.e. at_get
  fut->put_strand = t_current;

  // NB: Is it okay to do this here? Seems easier than during
  // at_get(), since a future may have get() called on it multiple
  // times.
  attachify(t_current);

  // The only way we know of a future continuation, currently.
  at_future_continuation(f, p);
}

} // namespace reach
