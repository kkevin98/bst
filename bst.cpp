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
  std::pair<iterator, bool> _insert(F&& x);

public:

  bst() = default;
  ~bst() = default;

  /* Return an iterator to the node containg the key x, if any; end() otherwise */
  iterator find(const key_type& x) noexcept {
    auto tmp = root.get();
    while (tmp)
      if (compare(x, tmp->key_val.first))
        tmp = tmp->left_child.get();
      else if (compare(tmp->key_val.first, x))
        tmp = tmp->right_child.get();
      else break;
    return tmp ? iterator{tmp} : end();
  }

  /* Return a const_iterator to the node containg the key x, if any; end() otherwise */
  const_iterator find(const key_type& x) const noexcept {
    auto tmp = root.get();
    while (tmp)
      if (compare(x, tmp->key_val.first))
        tmp = tmp->left_child.get();
      else if (compare(tmp->key_val.first, x))
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
    return _insert(x);
  }

  /* Insert a new node with key-value specified in x.
   * If a node with the specified key is already present the tree is not modified.
   * Return an <iterator, flag> pair where:
   *   iterator: indicate the node with the specified key;
   *   flag    : is true if a new node has been inserted, false otherwise. */
  std::pair<iterator, bool> insert(pair_type&& x) {
    return _insert(std::move(x));
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

};


template <typename K, typename V, typename Comp>
template <typename F>
std::pair<typename bst<K, V, Comp>::iterator, bool> bst<K, V, Comp>::_insert(F&& x) {
  bool inserted = true;
  if (!root) {
    root.reset(new node_t{std::forward<F>(x)});
    return {iterator{root.get()}, inserted};
  }
  node_t* parent = nullptr;
  node_t* tmp = root.get();
  while (tmp) {
    parent = tmp;
    if (compare(x.first, tmp->key_val.first))
      tmp = tmp->left_child.get();
    else if (compare(tmp->key_val.first, x.first))
      tmp = tmp->right_child.get();
    else {
      inserted = false;
      return {iterator{tmp}, inserted};
    }
  }
  return std::pair{iterator{parent->create_child(std::forward<F>(x))}, inserted};
}

template <typename K, typename V, typename Comp>
struct bst<K, V, Comp>::node_t {

  pair_type key_val;

  std::unique_ptr<node_t> left_child, right_child;

  node_t* parent;

  node_t(const pair_type& x) : key_val{x} {};

  node_t(pair_type&& x) : key_val{std::move(x)} {};

  node_t(const pair_type& x, node_t* const p) : node_t{x} { parent = p; }

  node_t(pair_type&& x, node_t* const p) : node_t{std::move(x)} { parent = p; }

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

  template <typename F>
  node_t* _create_child(F&& x) {
    //TODO: Add assert to ckeck that keys are not equal
    auto new_node = new node_t{std::forward<F>(x), this};
    if (Comp{}(x.first, key_val.first))
      left_child.reset(new_node);
    else
      right_child.reset(new_node);
    return new_node;
  }

  node_t* create_child(const pair_type& x) {
    return _create_child(x);
  }

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
  iterator& operator++() noexcept {
    current = current->next();
    return *this;
  }

  // post increment
  iterator& operator++(int) noexcept {
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

  for (const auto v : test) {
    std::cout << v << std::endl;
  }

  return 0;
}
