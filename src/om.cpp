om_ds::om_ds() : m_root(new tl_node()), m_height(0)
{
  m_root->below = new bl_list(m_root);
  m_root->level = MAX_LEVEL;
  m_root->num_leaves = 1;
  m_root->label = 0;
  m_root->parent = m_root->left = m_root->right = nullptr;

  // m_head = m_tail = m_root;

  // Initial insert
}

om_ds::~om_ds()
{
  // I don't think we actually need this...
}

node* om_ds::insert(node* base)
{

}

bool om_ds::precedes(const node* x, const node* y) const
{

}

void om_ds::fprint(FILE* out) const
{

}

void om_ds::verify();
{

}

tl_node* om_ds::get_tl(const node* n) const
{
  assert(n->list->above->level == om::MAX_LEVEL);
  return n->list->above;
}
