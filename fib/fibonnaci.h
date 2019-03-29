#ifndef _FIBONNACI_H_
#define _FIBONNACI_H_

#include <iostream>
#include <unordered_map>

using namespace std;


// T the type of item and 
// K is the type of key for each item
template<typename T, typename K>
struct fibnode {
  // constructor
  fibnode(const T& item, const K& key, fibnode<T, K> *next = NULL, fibnode<T, K> *prev = NULL) {
    this->item = item;
    this->key = key;
    this->next = next;
    this->prev = prev;
  }

  // new nodes should always have no parents or children since fib heap
  fibnode *parent = NULL;    
  fibnode *child = NULL;     // any one child of the current node 
  fibnode *next;             
  fibnode *prev;             
  K key;                  // stores the key of the node
  T item;                 // stores the item of the the node
  unsigned int num = 0;   // the number of nodes underneath
  bool mark = 0;          // determines if node is marked
};


/* 
  T the type of item and 
  K is the type of key for each item
  The user should maintain an unordered_map of all the nodes that have been added to the heap
  for O(1) complexity decrease-key.
  For Dijkstra's the key will be time taken to travel to each node, thus decreasing the time 
  will require finding the vertex key in an unordered_map which should have the appropriate 
  pointer to the node.
*/
template <typename T, typename K>
class FibonnaciHeap {
public:
  // constructor
  FibonnaciHeap() {
    heapSize = maxDeg = trees = marked = 0;
    min = NULL;
  }

  // insert a single tree into the FibHeap
  // return the pointer to the node added to heap
  fibnode<T, K> * insert(const T& item, const K& key);

  // union

  // Decrease the key of the node
  void decreaseKey(fibnode<T,K> * node, K val);

private:
  unsigned int heapSize;    // number of nodes
  unsigned int maxDeg;      // maximum degree of nodes in heap
  unsigned int trees;       // number of trees
  fibnode<T, K> *min;       // pointer to minimum key in fibHeap
  unsigned int marked;      // number of marked nodes 
};

template <typename T, typename K>
fibnode<T, K> * FibonnaciHeap<T, K>::insert(const T& item, const K& key) {

  fibnode<T, K> *current_node;

  // if the current insertion is the first
  if (heapSize == 0) {
    current_node = new fibnode<T, K>(item, key);
    current_node->prev = current_node;
    current_node->next = current_node;
    min = current_node;
  }

  // fix circular linked list for 
  else {
    current_node = new fibnode<T, K>(item, key, min, min->prev);
    min->prev->next = current_node;
    min->prev = current_node;
  }

  // now check if new insertion has smaller key than the current min
  // set new min to the new node 
  if (current_node->key < min->key) {
    min = current_node;
  }

  // increment heap size and tree count
  heapSize++;
  trees++;

  // for O(1) time decrease-key need a quick-lookup for vertex ID
  // return the pointer to added vertex
  return current_node;
}

template <typename T, typename K>
void FibonnaciHeap<T, K>::decreaseKey(fibnode<T,K> * node, K val) {
  
}

// node<T, K> * FibonnaciHeap<T, K>::decrease_key(const K& key) {
// }






#endif
