#include <iostream>
#include <utility>
#include <memory>
#include <iterator>

template <typename K, typename V, typename Comp=std::less<K>>
class bst {

  using key_type = K;
  using value_type = V;
  using pair_type = std::pair<const key_type, value_type>;

  struct node_t;

  std::unique_ptr<node_t> root;

  const Comp compare = Comp{};

public:

  template <typename VV>
  class _iterator;

  using iterator = _iterator<V>;
  using const_iterator = _iterator<const V>;

  bst() = default;
  ~bst() = default;

  /* Return an iterator to the node containg the key x, if any; end() otherwise */
  iterator find(const key_type& x) noexcept {
    auto tmp = root.get();
    while (tmp)
      if (compare(x, tmp->key_val.first()))
        tmp = tmp->left_child.get();
      else if (compare(tmp->key_val.first(), x))
        tmp = tmp->right_child.get();
      else break;
    return tmp ? iterator{tmp} : end();
  }

  /* Return a const_iterator to the node containg the key x, if any; end() otherwise */
  iterator find(const key_type& x) const noexcept {
    auto tmp = root.get();
    while (tmp)
      if (compare(x, tmp->key_val.first()))
        tmp = tmp->left_child.get();
      else if (compare(tmp->key_val.first(), x))
        tmp = tmp->right_child.get();
      else break;
    return tmp ? const_iterator{tmp} : end();
  }

  std::pair<iterator, bool> insert(const pair_type& x);

  std::pair<iterator, bool> insert(pair_type&& x);

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
struct bst<K, V, Comp>::node_t {

  pair_type key_val;

  std::unique_ptr<node_t> left_child, right_child;

  node_t* parent;

  /* Return a pointer to the node with the smallest key of the sub-tree starting from this node */
  node_t* get_minimun() const noexcept {
    auto tmp = this;
    while (tmp->left_child)
      tmp = tmp->left_child.get();
    return tmp;
  }

  /* Return true if the current node has a right child, false otherwise */
  bool has_right_child() const noexcept { return right_child; }

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

  _iterator(const _iterator& that) = default;

  reference operator*() const noexcept {
    return current->key_val.first;
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

  std::cout << "Ciao from bst data structure!!" << std::endl;

  return 0;
}
