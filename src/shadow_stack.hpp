#include "utils/stack.hpp"

#include <cassert>

template <typename T>
struct shadow_frame {
  enum frame_type : unsigned char { SPAWNER = 0, HELPER, FUTURE};

  T data;
  frame_type type;
  bool is_helper() const { return type == HELPER; }
}; // struct shadow_frame

template <typename T>
class shadow_stack : public utils::stack< shadow_frame<T> > {
  using frame = shadow_frame<T>;
  using base_type = utils::stack<frame>;
  using index_t = typename utils::stack< shadow_frame<T> >::index_t;
public:

  T* push() { utils::stack<frame>::push(); return this->head(); }
  T* head() const { return ancestor(0); }
  T* ancestor(int i) const {
    frame *f = base_type::ancestor(i);
    return &f->data;
  }

  //void push(typename frame::frame_type type);
  void push_spawner() {
    base_type::push();
    assert(!(base_type::ancestor(1)->is_helper()));
    base_type::head()->type = frame::frame_type::HELPER;
  }
  void push_helper() {
    base_type::push();
    base_type::head()->type = frame::frame_type::SPAWNER;
  }
  void push_future() {

  }
}; // class shadow_stack
