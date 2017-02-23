/** An efficient Order Maintenance data structure. See the SP-Order
    paper and related. This is only meant to work sequentially.
*/
#pragma once

#include <cstdio> // FILE
#include <climits>

namespace om {

typedef unsigned long label_t;

// Can these be moved inside om_ds??
static constexpr label_t MAX_LABEL = ULONG_MAX;
static constexpr label_t NODE_INTERVAL = (((label_t)1) << 58); // N / log_2
                                                          // = 2^58
static constexpr label_t SUBLIST_SIZE = ((label_t)64); // log_2 N = 64

/// @todo{What is MAX_LEVEL for?}
static constexpr label_t MAX_LEVEL = (sizeof(label_t) * 8);

class bl_list;
struct bl_node {
  label_t label;
  struct bl_node *next, *prev;
  bl_list* list;
};
typedef bl_node om_node;

struct tl_node {
  label_t label;
  size_t level;
  size_t num_leaves;

  struct tl_node* parent;

  /// Leaves don't need left/right pointers
  /// Internal nodes don't need prev/next pointers
  union {
    tl_node* left; // internal node
    tl_node* prev; // leaf
  };

  union {
    tl_node* right; // internal node
    tl_node* next; // leaf
  };

  bl_list* below;
}; // struct tl_node

class bl_list {
private:
  static constexpr label_t DEFAULT_INITIAL_LABEL = 0;
  typedef bl_node node;

  size_t m_size = 0;
  node* m_head = nullptr;
  node* m_tail = nullptr;
  tl_node* m_above = nullptr;
  //node m_initial_node;

public:
  bl_list(tl_node* above, label_t initial_label = DEFAULT_INITIAL_LABEL);
  ~bl_list();
  node* insert(node* base); // insert_initial?
  bool precedes(const node* x, const node* y) const;
  inline size_t size() const { return m_size; }
  void fprint(FILE* out) const;
  inline void print() const { return fprint(stdout); }
  inline tl_node* above() const { return m_above; }
  bool verify() const;

}; // class bl_list

class om_ds {
private:
  typedef bl_node node;
  
  tl_node* m_root;
  // tl_node* m_head;
  // tl_node* m_tail;
  size_t m_height;
  
  void relabel();
  label_t verify_subtree(tl_node* n) const;
  tl_node* get_tl(const node* n) const;

public:
  om_ds();
  ~om_ds();
  node* insert(node* base);
  bool precedes(const node* x, const node* y) const;
  node* base() const
  
  void fprint(FILE* out) const;
  inline void print() const { fprint(stdout); }
  void verify() const; // Make sure struct is valid
  
}; // class om_ds

} // namespace om
