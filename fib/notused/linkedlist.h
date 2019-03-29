#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <cassert>

using namespace std;

// struct for holding an item and pointers to the next and previous node
template <typename T>
struct ListNode {
  // constructor
  ListNode(const T& item, ListNode<T> *prev = NULL, ListNode<T> *next = NULL) {
    this->item = item;
    this->prev = prev;
    this->next = next;
  }

  T item;
  ListNode<T> *next, *prev;
};

// A linked list that circles back to itself
template <typename T>
class LinkedList {
public:
  LinkedList() {
    first = last = NULL;
    listSize = 0;
  }

  ~LinkedList() {
    // delete one item at a time until the list is empty
    while (size() > 0) {
      removeFront();
    }
  }

  void insert(const T& item);

  // assumes the node is in this list
  void removeNode(ListNode<T> *node);

  unsigned int size() const { return listSize; }

  // Find and return a pointer to the first node with the item.
  // Returns the NULL pointer if the item is not in the list.
  ListNode<T>* find(const T& item) const;

private:
  ListNode<T> *first, *last;
  unsigned int listSize;
};

template <typename T>
void LinkedList<T>::insert(const T& item) {
  // if the list is empty then the node must circle back to itself
  if (first == NULL) {
    ListNode<T> *node;
    node = new ListNode<T>(item, node, node);
    assert(node != NULL);
    last = node;
  }
  // if the list has an item
  // the previous node should be the last, and the next should be the current first node
  else {
    ListNode<T> *node = new ListNode<T>(item, last, first);
    assert(node != NULL);
    first->prev = node;
  }
}

template <typename T>
void LinkedList<T>::removeNode(ListNode<T>* node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;

  delete node;
  listSize--;
}

template <typename T>
ListNode<T>* LinkedList<T>::find(const T& item) const {
  // crawl along the list until the item is found
  ListNode<T>* node = first;
  while (node != NULL && node->item != item) {
    node = node->next;
  }

  // returns NULL if the item was not found
  return node;
}

#endif
