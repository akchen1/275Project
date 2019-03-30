#ifndef _FIBONNACI_H_
#define _FIBONNACI_H_

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

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
  K key;                    // stores the key of the node
  T item;                   // stores the item of the the node
  unsigned int deg = 0;     // the degree of the node *ONLY RELEVANT FOR ROOT NODES*
  unsigned int num = 0;     // the number of nodes underneath
  bool mark = 0;            // determines if node is marked
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
  // return minimum of heap
  fibnode<T, K> getMin(); 

  // insert a single tree into the FibHeap
  // return the pointer to the node added to heap
  fibnode<T, K> * insert(const T& item, const K& key);

  // removes the minimum from fibheap
  void popMin();

  // recursive meld will meld nodes until there are no repeats of degrees in the current roots
  // return the new root node of melded nodes
  fibnode<T, K> * recursiveMeld(fibnode<T, K> *node1, fibnode<T, K> *node2,
                                unordered_map<unsigned int, fibnode <T,K>*>& roots);

  /*consolidates (unionizes and combines) the fibHeap
    consolidate should only be run after a minimum element is popped
    when the function is run the min of the fibHeap is not the true minimum
    and thus consolidate also finds the new minimum of the heap*/
  void consolidate();

  // Decrease the key of the node
  void decreaseKey(fibnode<T,K> * node, K val);

private:
  unsigned int heapSize;    // number of nodes
  unsigned int maxDeg;      // maximum degree of nodes in heap
  unsigned int trees;       // number of trees
  fibnode<T, K> *start;     // start of loop for consolidate
  fibnode<T, K> *min;       // pointer to minimum key in fibHeap
  unsigned int marked;      // number of marked nodes 
};


template <typename T, typename K>
fibnode<T, K> FibonnaciHeap<T, K>::getMin() {

  // ensure tree is not empty and return dereferenced minimum
  assert(min != NULL);
  return *min;
}

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
fibnode<T, K> * FibonnaciHeap<T,K>::recursiveMeld(fibnode<T, K> *node1, fibnode<T, K> *node2,
                                    unordered_map<unsigned int, fibnode<T, K> *>& roots) {
  
  fibnode<T, K> *root;
  fibnode<T, K> *kid;
  // compare which key is minimum *case for first node smaller*
  if (node1->key < node2->key) {root = node1; kid = node2;} 
  else {root = node2; kid = node1;}

  if (kid == start) {start = root;}   // if the start is becoming child, set start to stay on root
    root->deg++;                     
    kid->next->prev = kid->prev;     // pop kid out of root list
    kid->prev->next = kid->next;

    if (root->child == NULL) {          // if there is no child yet
      root->child = kid;
      kid->next = kid;
      kid->prev = kid;
      kid->parent = root;
    }

    else {                               // child exists
      kid->parent = root;
      kid->next = root->child->next;
      kid->prev = root->child->prev;
      root->child->next->prev = kid;
      root->child->next = kid;
      root->child = kid;                // as long as root child points to a child it's ok; this is tidy
    }
  
  auto iter = roots.find(root->deg);
  if(iter == roots.end()) {
    roots[root->deg] = root;
  }
  else {
    fibnode<T, K>* foundNode = iter->second;
    roots.erase(iter);
    root = recursiveMeld(foundNode, root, roots);
  }

  return root;
}

template <typename T, typename K>
void FibonnaciHeap<T, K>::consolidate() {

  /* THE MINIMUM SHOULD CURRENTLY BE AN ARBITRARY ROOT NODE */
  start = min;                                // tracks node where to end consolidate
  fibnode<T, K> *current = start;             // start at the arb. root node
  fibnode<T, K> *foundNode;                   // if identical degrees exist
  // unordered map storing degrees, and their root node
  unordered_map<unsigned int, fibnode<T,K>* > roots; 
  
  // consolidate the heap until all nodes before minimum element is satisfied
  do {
    auto iter = roots.find(current->deg);     // find root degrees in fibHeap are identical to current
    if (iter == roots.end()) {                 // if it doesn't exist put into roots
      roots[current->deg] = current;
    }

    else {
      foundNode = iter->second;                               // get the identical node
      roots.erase(iter);                                      // erase the found 
      current = recursiveMeld(foundNode, current, roots);     // meld the nodes in question
    }

    current = current->next;
  } while (current != start);

}


template <typename T, typename K>
void FibonnaciHeap<T, K>::popMin() {

  // cannot popMin of empty heap
  assert(heapSize != 0);

  fibnode<T, K> *poppedMin = min; 

  // only the root node, delete minimum and decrement counters
  if(heapSize == 1) {
    delete min; 
    heapSize--;
    trees--;
    return;
  }

  // if the minimum has no child remove minimum and temporarily set minimum to old
  // minimum's next value *MAY NOT BE TRUE MINIMUM*
  else if(min->child == NULL) {
    min->prev->next = min->next;
    min->next->prev = min->prev;
    min = min->next;
  }

  // popped minimum has children
  else {
    // for each child of the popped node, add the node to the list of root nodes
    fibnode<T, K> *current = min->child;

    // if there is only one root, need to make the first child 
    // point back to itself in a doubly linked list
    if(trees == 1) {min->next = current; min->prev = current;}

    // After while loop may not be true minimum -- instead the child of the removed
    // node will be set as the minimum.
    do { 
      current = current -> next;
      current->parent = NULL;
      current->next = min->next;
      current->prev = min->prev;
      min->next->prev = current;
      min->prev->next = current;
      min = current;
    } while (poppedMin->child != current);        // check
  }

  // minimum has been popped so find new minimum and satisfy fib property
  consolidate();


  delete poppedMin;
}





template <typename T, typename K>
void FibonnaciHeap<T, K>::decreaseKey(fibnode<T,K> * node, K val) {
  
}

// node<T, K> * FibonnaciHeap<T, K>::decrease_key(const K& key) {
// }






#endif
