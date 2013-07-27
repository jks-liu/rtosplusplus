#ifndef LIST_H_
#define LIST_H_

class List {
public:
  struct ListHead {
    ListHead *next;
    ListHead *prev;
  };
  List(void): size_(0) {
    list_head_.next = &list_head_;
    list_head_.prev = &list_head_;
  }
  void add(ListHead *node) {
    list_add(node, &list_head_, list_head_.next);
  }
  int size(void) {return size_;}
  bool is_empty(void) {return size_ == 0;}
  ListHead *get(int count) {  /* Count nust < size_ */
    ListHead *lh = &list_head_;
    for (int i = 0; i < count + 1; ++i) {
      lh = lh->next;
    }
    return lh;
  }
  bool is_last(ListHead *node) {
    return node->next == &list_head_;
  }
private:
  ListHead list_head_;
  void list_add(ListHead *node,
                 ListHead *prev,
                 ListHead *next) {
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
    ++size_;
  }
  int size_;
};

#endif  /* LIST_H_ */





