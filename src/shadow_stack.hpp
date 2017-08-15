#include "utils/stack.hpp"

#include <cassert>

// XXX: Why do I need different frame types, again?
template <typename T>
struct shadow_frame {
  //enum frame_type : unsigned char { SPAWNER = 0, HELPER};

  T data;
  //frame_type type;
  //bool is_helper() const { return type == HELPER; }
}; // struct shadow_frame

template <typename T>
class shadow_stack : public utils::stack< shadow_frame<T> > {
  using frame = shadow_frame<T>;
  using base_type = utils::stack<frame>;
  using index_t = typename utils::stack< shadow_frame<T> >::index_t;

public:

  T* push() { base_type::push(); return this->head(); }
  T* head() const { return ancestor(0); }
  T* parent() const { return ancestor(1); }
  T* ancestor(int i) const {
    frame *f = base_type::ancestor(i);
    return &f->data;
  }

  //void push(typename frame::frame_type type);
  // T* push_spawner() {
  //   T* ret = push();
  //   assert(!(base_type::ancestor(1)->is_helper()));
  //   base_type::head()->type = frame::frame_type::HELPER;
  //   return ret;
  // }
  // T* push_helper() {
  //   T* ret = push();
  //   base_type::head()->type = frame::frame_type::SPAWNER;
  //   return ret;
  // }
  
}; // class shadow_stack
