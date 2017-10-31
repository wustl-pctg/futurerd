#include "reach_structured.hpp"
#include <cassert>
#include <iostream>
//#define LOG fprintf(stderr, "In %s\n", __PRETTY_FUNCTION__);
#define LOG

namespace reach {

structured::structured(sframe_data *initial) { create_strand(initial); }

structured::smem_data* structured::active(sframe_data *f) { return f->Sbag; }

/********** Parallelism Creation **********/
void structured::begin_strand(sframe_data *f, sframe_data *p) {
  f->Sbag = p->Sbag;
  f->Pbag = nullptr;
}

void structured::create_strand(sframe_data *f) {
  f->Sbag = new spbag(spbag::bag_kind::S);
  f->Pbag = nullptr; // will create this lazily when we need it
}

void structured::at_future_create(sframe_data *f) { create_strand(f); }
void structured::at_spawn(sframe_data *f) { LOG; create_strand(f); }

/********** Parallelism Deletion **********/
void structured::at_future_get(sframe_data *f, sfut_data *fut) {
  assert(fut->put_strand);
  f->Sbag->merge(fut->put_strand);
}

// A sync is equivalent to calling "get" on all the futures that were
// "spawned" in this block
void structured::at_sync(sframe_data *f) { LOG;
  // make sure it's a real sync (XXX: ???)
  if (!f->Pbag) return; 

  f->Sbag->merge(f->Pbag);
  f->Pbag = nullptr;
}

/********** Continuations **********/
// Returning from a spawned function or future function
// f is the spawned/created frame, p is the parent frame
void structured::continuation(sframe_data *f, sframe_data *p) {
  // XXX: can't we just change f->Sbag to be a bag, then assign it to p->Pbag?
  // if (!p->Pbag) p->Pbag = new spbag(spbag::bag_kind::P);
  // else p->Pbag->set_kind(spbag::bag_kind::P);
  // p->Pbag->merge(f->Sbag);

  if (!p->Pbag) 
    p->Pbag = f->Sbag;
  else
    p->Pbag->merge(f->Sbag);
  // sometimes this bag may have been merged with an S-bag, so it
  // think it's an Sbag. But now it must be a Pbag. See basic/test6.
  // XXX: is there a cleaner way to do this?
  spbag::find(p->Pbag)->set_kind(spbag::bag_kind::P);
  //p->Pbag->set_kind(spbag::bag_kind::P);

  // XXX: can't we just change f->Sbag to be a Pbag, rather than
  // creating a new bag?
  // if (!f->Pbag) f->Pbag = new spbag(spbag::bag_kind::P);
  // f->Pbag->merge(f->Sbag);
}

void structured::at_spawn_continuation(sframe_data *f, sframe_data *p)
{ LOG; continuation(f,p); }

void structured::at_future_finish(sframe_data *f, sframe_data *p, sfut_data *fut) {
  continuation(f, p);
  fut->put_strand = p->Pbag;
}

} // namespace reach
