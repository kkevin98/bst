#include <iostream>
#include <utility>
#include <memory>
#include <iterator>

template <typename K, typename V, typename Comp=std::less<K>>
class bst {

  struct node_t;

  template <typename VV>
  class _iterator;

public:

  using key_type = K;
  using value_type = V;
  using pair_type = std::pair<const key_type, value_type>;
  using iterator = _iterator<V>;
  using const_iterator = _iterator<const V>;

private:

  // member vars
  std::unique_ptr<node_t> root;
  const Comp compare = Comp{};

  // member functions
  template <typename F>
  std::pair<iterator, bool> _lazy_insert(F&& x);

  template <typename... Ts>
  std::pair<iterator, bool> _insert(Ts&&... x);

public:

  bst() = default;
  ~bst() = default;

  /* Return an iterator to the node containg the key x, if any; end() otherwise */
  iterator find(const key_type& x) noexcept {
    auto tmp = root.get();
    while (tmp)
      if (compare(x, tmp->key()))
        tmp = tmp->left_child.get();
      else if (compare(tmp->key(), x))
        tmp = tmp->right_child.get();
      else break;
    return tmp ? iterator{tmp} : end();
  }

  /* Return a const_iterator to the node containg the key x, if any; end() otherwise */
  const_iterator find(const key_type& x) const noexcept {
    auto tmp = root.get();
    while (tmp)
      if (compare(x, tmp->key()))
        tmp = tmp->left_child.get();
      else if (compare(tmp->key(), x))
        tmp = tmp->right_child.get();
      else break;
    return tmp ? const_iterator{tmp} : end();
  }

  /* Insert a new node with key-value specified in x.
   * If a node with the specified key is already present the tree is not modified.
   * Return an <iterator, flag> pair where:
   *   iterator: indicate the node with the specified key;
   *   flag    : is true if a new node has been inserted, false otherwise. */
  std::pair<iterator, bool> insert(const pair_type& x) {
    return _lazy_insert(x);
  }

  /* Insert a new node with key-value specified in x.
   * If a node with the specified key is already present the tree is not modified.
   * Return an <iterator, flag> pair where:
   *   iterator: indicate the node with the specified key;
   *   flag    : is true if a new node has been inserted, false otherwise. */
  std::pair<iterator, bool> insert(pair_type&& x) {
    return _lazy_insert(std::move(x));
  }

  /* Constuct a new node with the given arguments and append it to the tree.
   * If a node with the key equial to the one of the new node is already
   * present, the tree is not modified.
   * Return an <iterator, flag> pair where:
   *   iterator: indicate the node with the specified key;
   *   flag    : is true if a new node has been inserted, false otherwise. */
  template <typename... Ts>
  std::pair<iterator, bool> emplace(Ts&&... args) {
    return _insert(std::forward<Ts>(args)...);
  }


  iterator begin() noexcept {
    return root ? iterator{root->get_minimum()} : iterator{nullptr};
  }
  iterator end() noexcept { return iterator{nullptr}; }

  const_iterator begin() const noexcept {
    return root ? const_iterator{root->get_minimum()} : const_iterator{nullptr};
  }
  const_iterator end() const noexcept { return const_iterator{nullptr}; }

  const_iterator cbegin() const noexcept {
    return root ? const_iterator{root->get_minimum()} : const_iterator{nullptr};
  }
  const_iterator cend() const noexcept { return const_iterator{nullptr}; }

  friend std::ostream& operator<<(std::ostream& os, const bst& x) {
    auto it = x.cbegin();
    auto last = x.cend();
    if (it == last)
      os << "Tree is empty";
    else while (it!=last) {
      os << *it << ", ";
      ++it;
    }
    return os;
  }
};

/* Return a pair of an iterator pointing to the node with the specified
 * key and a bool representing the result of the insertion.
 * The bool is false (no insertion is performed) in case a node with
 * the specified key is alredy present in the tree and itertor points
 * to that exact node.
 * The bool is true (thus an newly node has been inserted) in case a
 * node with the specified key is not already present in the tree. In
 * this case the returned iterator points to the newly inserted node.
 *
 * Note: A new node is created if and only if the given key is not already
 *       present in the tree. */
template <typename K, typename V, typename Comp>
template <typename F>
std::pair<typename bst<K, V, Comp>::iterator, bool> bst<K, V, Comp>::_lazy_insert(F&& x) {
  bool inserted = true;
  if (!root) {
    root.reset(new node_t{std::forward<F>(x)});
    return {iterator{root.get()}, inserted};
  }
  node_t* parent = nullptr;
  node_t* tmp = root.get();
  while (tmp) {
    parent = tmp;
    if (compare(x.first, tmp->key()))
      tmp = tmp->left_child.get();
    else if (compare(tmp->key(), x.first))
      tmp = tmp->right_child.get();
    else {
      inserted = false;
      return {iterator{tmp}, inserted};
    }
  }
  return std::pair{iterator{parent->create_child(std::forward<F>(x))}, inserted};
}

/* Construct a node with the given arguments and possibly add it to the
 * tree. If the newly constructed node turns out having the same key of
 * a node already present in the tree, it is destroyed and the insertion
 * does not take place; it inserted otherwise.
 * This function return a pair consisting of:
 *   - an iterator pointing to the node with the same key as the one
 *     constructed with the given arguments
 *   - a bool which is true if a new node has been inserted in the tree,
 *     false otherwise.
 *
 * Note: A new node is created indipendently if its key is already
 *       present in the tree or not. */
template <typename K, typename V, typename Comp>
template <typename... Ts>
std::pair<typename bst<K, V, Comp>::iterator, bool> bst<K, V, Comp>::_insert(Ts&&... x) {
  node_t* new_node = new node_t{std::forward<Ts>(x)...};

  bool inserted = true;
  if (!root) {
    root.reset(new_node);
    return {iterator{root.get()}, inserted};
  }
  node_t* parent = nullptr;
  node_t* tmp = root.get();
  while (tmp) {
    parent = tmp;
    if (compare(new_node->key(), tmp->key()))
      tmp = tmp->left_child.get();
    else if (compare(tmp->key(), new_node->key()))
      tmp = tmp->right_child.get();
    else {
      inserted = false;
      delete new_node;
      return {iterator{tmp}, inserted};
    }
  }
  return std::pair{iterator{parent->add_child(new_node)}, inserted};
}

template <typename K, typename V, typename Comp>
struct bst<K, V, Comp>::node_t {

  pair_type key_val;

  std::unique_ptr<node_t> left_child, right_child;

  node_t* parent;

  node_t(const pair_type& x) : key_val{x} {};

  node_t(pair_type&& x) : key_val{std::move(x)} {};

  template <typename... Ts>
  node_t(Ts&&... args) : key_val{std::forward<Ts>(args)...} {};

  const K& key() const noexcept { return key_val.first; }

  /* Return a pointer to the node with the smallest key of the sub-tree starting from this node */
  node_t* get_minimum() noexcept {
    auto tmp = this;
    while (tmp->left_child)
      tmp = tmp->left_child.get();
    return tmp;
  }

  /* Return a pointer to the node with the smallest key of the sub-tree starting from this node */
  const node_t* get_minimum() const noexcept {
    auto tmp = this;
    while (tmp->left_child)
      tmp = tmp->left_child.get();
    return tmp;
  }

  /* Return true if the current node has a right child, false otherwise */
  bool has_right_child() const noexcept { return right_child!=nullptr; }

  /* Return the node with the smallest key greater then the key of the current node */
  node_t* next() const {
    if (has_right_child())
      return right_child->get_minimum();
    auto tmp = this;
    auto tmp_parent = this->parent;
    while (tmp_parent && tmp_parent->right_child.get()==tmp) {
      tmp = tmp_parent;
      tmp_parent = tmp->parent;
    }
    return tmp_parent;
  }

  /* Set the node pointed to by 'x' as left of right child for the
   * current node according to their keys */
  node_t* add_child(node_t* x) noexcept {
    //TODO: Add assert to check that node hasn't a parent yet
    //TODO: Add assert to check that childs are not alredy occupied
    //TODO: Add assert to ckeck that keys are not equal
    x->parent = this;
    if (Comp{}(x->key(), this->key()))
      left_child.reset(x);
    else
      right_child.reset(x);
    return x;
  }

  template <typename F>
  node_t* _create_child(F&& x) {
    return add_child(new node_t{std::forward<F>(x)});
  }

  /* Construct and add a child to the current node given the key-value
   * pair 'x' */
  node_t* create_child(const pair_type& x) {
    return _create_child(x);
  }

  /* Construct and add a child to the current node given the key-value
   * pair 'x' */
  node_t* create_child(pair_type&& x) {
    return _create_child(std::move(x));
  }

};

template <typename K, typename V, typename Comp>
template <typename VV>
class bst<K, V, Comp>::_iterator{
  using node_t = typename bst<K, V, Comp>::node_t;
  node_t* current;
public:
  using value_type = VV;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  using reference = value_type&;
  using pointer = value_type*;

  explicit _iterator(node_t* const ptr): current{ptr} {}

  reference operator*() const noexcept {
    return current->key_val.second;
  }

  pointer operator->() const noexcept {
    return &**this;
  }

  // pre-increment
  _iterator& operator++() noexcept {
    current = current->next();
    return *this;
  }

  // post increment
  _iterator& operator++(int) noexcept {
    auto tmp{*this};
    ++(*this);
    return tmp;
  }

  friend bool operator==(const _iterator& lhs, const _iterator& rhs) noexcept {
    return lhs.current == rhs.current;
  }

  friend bool operator!=(const _iterator& lhs, const _iterator& rhs) noexcept {
    return !(lhs==rhs);
  }
};

int main() {

  auto test = bst<int, double>();

  auto find_result = test.find(0);

  std::cout << test << std::endl;

  if (find_result != test.end())
    std::cout << "Something went wrong while finding on an empty tree" << std::endl;

  auto insertion_result = test.insert({1, 12});

  if (insertion_result.second == false)
    std::cout << "Something wrong happened while adding root" << std::endl;

  insertion_result = test.insert({10, 1});
  insertion_result = test.insert({5, 2});
  insertion_result = test.insert({0, 100});

  if (insertion_result.second == false)
    std::cout << "Something wrong happened while adding nodes" << std::endl;

  insertion_result = test.insert({1, 27});

  if (insertion_result.second == true)
    std::cout << "Something wrong happened while modifing root" << std::endl;

  std::cout << test << std::endl;

  std::cout << "---- Test emplace ----" << std::endl;

  test.emplace(3, 1.5);

  std::cout << test << std::endl;

  return 0;
}
