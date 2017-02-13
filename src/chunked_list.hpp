#include <cstddef>
#include <cstdlib>

template <typename T>
class chunked_list {

  struct chunk {
    T* data;
    struct chunk* previous;
    std::size_t size;

    chunk() = delete;
    chunk(std::size_t sz, chunk* prev = nullptr)
      : previous(prev), size(sz)
    { data = (T*) malloc(sz * sizeof(T)); }
    ~chunk() { delete[] data; }
  }; // struct chunk
  
  static constexpr std::size_t DEFAULT_SIZE = 128;
  
  chunk* m_current = nullptr;
  std::size_t m_index = 0;

public:
  chunked_list() { m_current = new chunk(DEFAULT_SIZE); }
  ~chunked_list()
  {
    while (m_current) {
      chunk* prev = m_current->previous;
      delete m_current;
      m_current = prev;
    }
  }
  T* getNext()
  {
    T* slot = &m_current->data[m_index++];
    if (m_index == m_current->size)
      m_current = new chunk(m_current->size * 2, m_current);
    return slot;
  }
}; // class chunked_list
