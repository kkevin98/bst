#include <iostream>
#include <utility>
#include <memory>
#include <iterator>

template<typename K, typename V, typename Comp=std::less<K>>
class bst {
  
  struct node_t {
    std::pair<const K, V> key_val;
    std::unique_ptr<node_t> left_child, right_child;
  };

  std::unique_ptr<node_t> head;

public:
  class Iterator{
    node_t* current;
  public:
    using value_type = V;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type&;
    using pointer = value_type*;
  };

};

int main() {

  std::cout << "Ciao from bst data structure!!" << std::endl;

  return 0;
}
