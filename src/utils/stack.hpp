// A simple stack to trace the Cilk runtime stack.
// Based on the stack written by Tao Shardl for the Cilksan project.

#pragma once

//#include "util.hpp"
#include <cassert>
#include <cstdint> // int types like uint32_t, uint64_t, etc.
#include <cstring> // memset

namespace utils {

template <typename T>
class stack {
public:
  typedef uint32_t index_t;
protected:

  ///< Default capacity for call stack
  static constexpr uint64_t DEFAULT_CAPACITY = 128;

  T* m_stack;
  index_t m_cap;
  index_t m_head;
  index_t m_tail;

  void resize(index_t new_cap)
  {
    T* old_stack = m_stack;
    m_stack = new T[new_cap];
    
    index_t copy_end = m_cap > new_cap ? new_cap : m_cap;
    for (int i = 0; i < copy_end; ++i)
      m_stack[i] = old_stack[i];
    m_cap = new_cap;

    delete[] old_stack;
  }

  void double_cap() { resize(m_cap * 2); }
  void halve_cap() { resize(m_cap / 2); }

public:

  stack() : m_cap(DEFAULT_CAPACITY)
  {
    reset();
    m_stack = new T[m_cap];
  }

  ~stack() { delete[] m_stack; }

  void reset()
  {
    m_head = (index_t)-1;
    m_tail = (index_t)0;
  }

  T* push()
  {
    ++m_head;
    if (m_head == m_cap)
      double_cap();
    memset(&m_stack[m_head], 0, sizeof(T));
    return &m_stack[m_head];
  }

  void pop()
  {
    --m_head;
    if (m_cap > DEFAULT_CAPACITY && m_head < m_cap / 2)
      halve_cap();
  }

  /* Retrives an arbitrary ancestor's stack data. */
  T* ancestor(index_t i) const
  {
    assert(i <= m_head);
    assert(m_head < m_cap);
    return &(m_stack[m_head - i]);
  }

  T* at(index_t i) const
  {
    assert(i >= 0 && i <= m_head);
    assert(m_head < m_cap);
    return &(m_stack[i]);
  }

  T* head() const { return ancestor(0); }
  index_t size() const { return m_head + 1; }
  bool empty() const { return size() == 0; }
}; // class stack

} // namespace utils
