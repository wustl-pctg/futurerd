#include "om.hpp"
#include <cassert>

/// Assume: node is allocated and has label
static void bl_list::insert_internal(node* base, node* n)
{
  assert(n->label > base->label);

  if (base != m_tail) {// Not inserting at end of list
    assert(base->next);
    base->next->prev = n;
  } else {
    assert(base->next == nullptr);
    m_tail = n;
  }

  if (base == m_tail) { // inserting at end
    assert(base->next == nullptr);
    m_tail = n;
  } else {
    assert(base->next);
    base->next->prev = n;
  }
  n->next = base->next;
  base->next = n;
  n->prev = n;
  n->list = this;
  
}

bl_list::bl_list(tl_node* above, label_t initial_label) :
    m_size(0), m_above(above)
{
  node* n = new node();
  assert(n);
  n->label = initial_label;
  n->next = n->prev = nullptr;
  n->list = this;
  m_tail = m_head = n;
}

bl_list::~bl_list()
{
  // It's not really necessary for us to delete memory...
  // node* it = m_head;
  // while (it) {
  //   it = it->next;
  //   delete it->prev;
  // }
  // delete m_tail;
}

static label_t bl_list::get_new_label(node* base)
{
  label_t succ_label;
  if (base->next) {
    assert(base->next->label > base->label);
    succ_label = base->next->label;
  } else
    succ_label = MAX_LABEL;

  // Average them
  label_t label = (base->label / 2) + (succ_label / 2);

  // Correction for averaging two odd integers (integer division
  // rounding)
  /// @todo{Does this case ever happen?}
  if ((base->label & next_label & 0x1) == 0x1) label++;

  // Correction for adding to the end
  if (!base->next) label++;

  assert(label > base->label);
  assert(base->next == nullptr || label < base->next->label);

  return label;
}

node* bl_list::insert(node* base)
{
  assert(base);
  if (base->label == MAX_LABEL
      || (base->next && base->next->label == base->label + 1))
    return nullptr;

  node* n = new node();
  assert(n);
  n->label = get_new_label(base);
  insert_internal(base, n);
}

bool bl_list::precedes(const node* x, const node* y) const
{
  assert(x->list == y->list);
  return x->label < y->label;
}

bool bl_list::bl_verify() const
{
  if (m_head == nullptr) {
    assert(m_tail == nullptr);
    return true;
  } 
  assert(m_head->prev == nullptr);
  
  node* n = m_head;
  while (n->next) {
    assert(n->label < n->next->label);
    assert(n->list == this);
    assert(n->next->prev == n);
    
    n = n->next;
  }

  assert(m_tail && n == m_tail);
  assert(m_tail->list == this);
  assert(m_tail->next == nullptr);

  return true;
}

void bl_list::fprint(FILE* out) const
{
  node* current = m_head;
  size_t size = 0;
  fprintf(out, "Blist at %p\n", this);
  while (current) {
    fprintf(out, "->%zu", current->label);
    current = current->next;
  }
  fprintf(out, "\nSize: %zu\n", m_size);
}
