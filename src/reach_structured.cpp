#include "reach_structured.hpp"

namespace reach {

structured::structured(sframe_data *initial) {
  initial->Sbag = new spbag(spbag::bag_kind::S);
  initial->Pbag = nullptr; // will create this lazily when we need it
}

void at_cilk_function_start(, sframe_data *parent) {
//   if (!parent) { // first frame


/********** Parallelism Creation **********/
void structured::new_function(sframe_data *f) {
  f->Sbag = new spbag(spbag::bag_kind::S);
  f->Pbag = nullptr; // will create this lazily when we need it
}

void at_future_create(sframe_data *f) { new_function(f); }
void at_spawn(sframe_data *f) { new_function(f); }

/********** Parallelism Deletion **********/
void at_future_get(sframe_data *f, sfut_data *fut) {
  assert(fut->put_strand);
  f->Sbag->merge(fut->put_strand);
}

// A sync is equivalent to calling "get" on all the futures that were
// "spawned" in this block
void at_sync(sframe_data *f) {
  // make sure it's a real sync (XXX: ???)
  if (!f->Pbag) return; 

  f->Sbag->merge(f->Pbag);
  f->Pbag = nullptr;
}

/********** Continuations **********/
// Returning from a spawned function or future function
// f is the spawned/created frame, p is the parent frame
void continuation(sframe_data *f, sframe_data *p) {
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

void at_spawn_continuation(sframe_data *f, sframe_data *p)
{ continuation(f,p); }

void at_future_finish(sframe_data *f, sframe_data *p, sfut_data *fut) {
  continuation(f, p);
  fut->put_strand = p->Pbag;
}

/********** Cilk functions begin/end **********/
// void at_cilk_function_start(sframe_data *f, sframe_data *parent) {
//   if (!parent) { // first frame
//     f->Sbag = new spbag(spbag::bag_kind::S);
//     f->Pbag = nullptr; // will create this lazily when we need it
//   } else {
//     //*f = *parent;
//     f->Sbag = parent->Sbag;
//   }
// }

// void at_cilk_function_end(sframe_data *f, sframe_data *p) {
//   assert(f->Sbag);
//   if (!p) return; // last frame
  
//   // we may have strands saved in the parent's Pbag. In other words,
//   // we may be unsynced.

//   p->Sbag = f->Sbag;

//   if (f->Pbag) {
//     assert(f->Pbag->kind == spbag::bag_kind::P);
//     if (!p->Pbag) {
//       p->Pbag = f->Pbag;
//     } else {
//       assert(p->Pbag->kind == spbag::bag_kind::P);
//       p->Pbag->merge(f->Pbag);
//     }
//   }
// }

} // namespace reach
