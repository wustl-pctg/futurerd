#include "utils/stack.hpp"

#include <cassert>

template <typename T>
struct shadow_frame {
  T data;
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
}; // class shadow_stack
