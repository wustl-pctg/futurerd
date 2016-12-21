template <typename T>
class ChunkedList {

  struct chunk {
    T* data;
    struct chunk* previous;
    std::size_t size;

    chunk() = delete;
    chunk(std::size_t sz, chunk* prev = nullptr)
      : size(sz), previous(prev)
    { data = new T[sz]; }
    ~chunk() { delete[] data; }
  }; // struct chunk
  
  static constexpr DEFAULT_SIZE = 128;
  
  chunk* m_current = nullptr;
  std::size_t m_index = 0;

public:
  ChunkedList() { m_current = new chunk(DEFAULT_SIZE); }
  ~ChunkedList()
  {
    while (m_current) {
      chunk* prev = m_current->prev;
      delete m_current;
      m_current = prev;
    }
  }
  T* getNext()
  {
    T* slot = &m_current->data[m_index++];
    if (m_index == m_current->size)
      m_current = new chunk(m_current->size * 2, m_current);
  }
}; // class ChunkedList
