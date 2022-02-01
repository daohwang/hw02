/* 基于智能指针实现双向链表 */
#include <cstdio>
#include <memory>

template <typename T>
struct Node {
  // 这两个指针会造成什么问题？请修复
  std::unique_ptr<Node> next;
  Node* prev;
  // 如果能改成 unique_ptr 就更好了!

  int value;

  // 这个构造函数有什么可以改进的？
  explicit Node(int val = 0) : prev(nullptr), value(val) {}
  Node(const Node& rhs) = delete;
  Node& operator=(const Node& rhs) = delete;
  Node(Node&& rhs)                 = default;
  Node& operator=(Node&& rhs) = default;

  /**
   * @brief 在当前节点之后插入val
   *
   * @param val
   */
  void
  insert(T val) {
    auto node  = std::make_unique<Node>(val);
    node->next = std::move(next);
    if (node->next) {
      node->next->prev = node.get();
    }
    node->prev = this;
    next       = std::move(node);
  }

  // 将当前节点删除
  void
  erase() {
    if (next)
      next->prev = prev;
    std::unique_ptr<Node> next_bak = std::move(next);
    if (prev)
      prev->next = std::move(next_bak);
  }

  ~Node() {
    printf("~Node()\n");  // 应输出多少次？为什么少了？
  }
};

template <typename T>
struct List {
  Node<T> dummy_head;

  List() = default;

  List(List const& other) {
    printf("List 被拷贝！\n");
    if (this == &other)
      return;

    Node<T>* curr = other.front();
    while (curr->next) {
      curr = curr->next.get();
    }
    while (curr != &other.dummy_head) {
      push_front(curr->value);
      curr = curr->prev;
    }
  }

  List& operator=(List const&) = delete;  // 为什么删除拷贝赋值函数也不出错？

  List(List&&)  = default;
  List& operator=(List&&) = default;
  ~List()                 = default;

  Node<T>*
  front() const {
    return dummy_head.next.get();
  }

  T
  pop_front() {
    T ret = dummy_head.next->value;
    dummy_head.next->erase();
    return ret;
  }

  void
  push_front(T value) {
    dummy_head.insert(value);
  }

  Node<T>*
  at(size_t index) const {
    auto curr = front();
    for (size_t i = 0; i < index; i++) {
      curr = curr->next.get();
    }
    return curr;
  }
};

template <typename T>
void
print(const List<T>& lst) {  // 有什么值得改进的？
  printf("[");
  for (auto curr = lst.front(); curr; curr = curr->next.get()) {
    printf(" %d", curr->value);
  }
  printf(" ]\n");
}

int
main() {
  List<int> a;

  a.push_front(7);
  a.push_front(5);
  a.push_front(8);
  a.push_front(2);
  a.push_front(9);
  a.push_front(4);
  a.push_front(1);

  print(a);  // [ 1 4 9 2 8 5 7 ]

  a.at(2)->erase();

  print(a);  // [ 1 4 2 8 5 7 ]

  List<int> b = a;

  a.at(3)->erase();

  print(a);  // [ 1 4 2 5 7 ]
  print(b);  // [ 1 4 2 8 5 7 ]

  b = {};
  a = {};

  return 0;
}
