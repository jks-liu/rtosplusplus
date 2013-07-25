#ifndef LIST_H_
#define LIST_H_

class List {
public:
  struct ListHead {
    ListHead *next;
    ListHead *prev;
  };
  List(void) {
    list_head_.next = &list_head_;
    list_head_.prev = &list_head_;
  }
  void add(ListHead *new) {
    list_add(new, &list_head_, list_head_.next);
  }
  
private:
  ListHead list_head_;
  void list_add(ListHead *new,
                 ListHead *prev,
                 ListHead *next) {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
  }
}

#endif  /* LIST_H_ */

