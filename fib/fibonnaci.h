#ifndef _FIBONNACI_H_
#define _FIBONNACI_H_

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>

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
  unsigned int maxDeg = 0;
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

  ~FibonnaciHeap() {
    while (heapSize > 0) {
      popMin();
    }
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

  // Doesn't use recursion and instead loops elements passed in the meld using a vector 
  // of degrees.
  void quickMeld(fibnode<T, K> *current, vector<fibnode<T, K> *> &degTable);

  /*consolidates (unionizes and combines) the fibHeap
    consolidate should only be run after a minimum element is popped
    when the function is run the min of the fibHeap is not the true minimum
    and thus consolidate also finds the new minimum of the heap*/

  void Consolidate();

  // Decrease the key of the node to value
  void decreaseKey(fibnode<T,K> * node, K val);

  // cut functions
  // cascade cut will return the adult of the node cut
  fibnode<T, K> * cut(fibnode<T, K> *node, fibnode<T, K> *adult);
  // fibnode<T, K> * cascade_cut(fibnode<T, K> *node);
  void info(const fibnode<T,K>  *current_node);

private:
  unsigned int heapSize;    // number of nodes
  unsigned int maxDeg;      // maximum degree of nodes in heap
  unsigned int trees;       // number of trees (along root)
  fibnode<T, K> *start;     // start of loop for consolidate
  fibnode<T, K> *min;       // pointer to minimum key in fibHeap
  unsigned int marked;      // number of marked nodes //
};

template <typename T, typename K>
void FibonnaciHeap<T,K>::info(const fibnode<T,K>  *current_node) {
  // cout << current_node->key << " at location "<< &current_node << endl;
  cout << "degree of : " << current_node->deg << endl;
  // if (current_node->child != NULL) {
  // cout << "if child : " << current_node->child->key << endl;
  // }
  // cout << "my parent is " << &current_node->parent << " " << current_node->key << endl;
  // cout << "my child is " << &current_node->child<< " " << current_node->key << endl;
  // cout << "the next is " << &current_node->next << " " << current_node->key<< endl;
  // cout << "the prev is " << &current_node->prev << " " << current_node->key<< endl;
}

template <typename T, typename K>
fibnode<T, K> FibonnaciHeap<T, K>::getMin() {

  // ensure tree is not empty and return dereferenced minimum
  assert(min != NULL);
  cout << "returning min" << endl;
  info(min);
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
    current_node = new fibnode<T, K>(item, key, min->next, min);
    min->next->prev = current_node;
    min->next = current_node;
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
    // info(current_node);
    // cout << "return min" << endl;
    // info(min);
  return current_node;
}


template <typename T, typename K>
void FibonnaciHeap<T, K>::popMin() {
      fibnode<T,K> * node = min;

  for (int i = 0; i< trees; i++) {
    cout << " -> " << node->key;
    node = node->next;
  }
  cout << endl;

  if (min->child != NULL) {
    node = min->child;
    for (int i = 0; i< min->deg; i++) {
    cout << " -> " << node->key;
    node = node->next;
  }

  }
  cout << endl;
  // cannot popMin of empty heap
  assert(heapSize != 0);

  fibnode<T, K> *poppedMin = min; 
  cout << "popping " << min->key << " at location " << &min << endl;
  info(min);
  // only the root node, delete minimum and decrement counters
  if(heapSize == 1) {

    delete min; 
    heapSize--;
    trees--;
    return;
  }
  

  else if(min->child == NULL) {
    (min->prev)->next = min->next;
    (min->next)->prev = min->prev;
    min = min->next;

  }
  else if (trees == 1) { // single tree only
    if (min->deg == maxDeg) {maxDeg--;}
    fibnode<T, K> *current = min->child;
    current->parent = NULL;

    for(unsigned int i = 0; i < min->deg-1; i ++) {
      current = current->next;
      current->parent = NULL;
    }

    trees += min->deg;
    min = current;
  }
  // popped minimum has children
  else {
    // keep track of node
    fibnode<T, K> *current = min->child;
    if (min->deg == maxDeg) {maxDeg--;}


    // move the linked children to the root nodes
    // (min->prev)->next = current;
    // current->prev = min->prev;

    current->parent = NULL;
    current->prev = min->prev;
    current->prev->next = current;


    for(unsigned int i = 0; i < min->deg-1; i++) {
      current = current->next;
      current->parent = NULL;
    }

    current->next = min->next;
    current->next->prev = current;

    // (min->next)->prev = current;
    // current->next = min->next;    
    trees += min->deg;
    // set arbritrary minimum as last element of the children
    min = current;
  }

  trees--;
  delete poppedMin;
  heapSize--;

  cout << " minimum key before consolidate entrance " << min->key << endl;



  // minimum has been popped so find new minimum and satisfy fib property
  Consolidate();
}

template <typename T, typename K>
fibnode<T, K> * FibonnaciHeap<T, K>::cut(fibnode<T, K> *node, fibnode<T, K> *adult) {
  trees++;

  // mark the cut unless it's a root 
  if (adult->parent != NULL) {
    adult->mark = true;       
  }
  node->mark = false;

  if (adult->deg < 2) {                 // the node cut is the only node
    adult->child = NULL;
    node->parent = NULL;
  }

  else if (adult->child == node) {      // disown the kid
    adult->child = node->next;
    node->parent = NULL;
  }

  // pop out the node and put into roots
  (node->next)->prev = node->prev;
  (node->prev)->next = node->next;

  (min->next)->prev = node;
  node->next = min->next;
  node->prev = min;
  min->next = node;

  if (adult->deg == maxDeg) {
    maxDeg--;
    adult->deg--;
  }
  else {adult->deg--;}

  return adult;
}

template <typename T, typename K>
void FibonnaciHeap<T, K>::decreaseKey(fibnode<T,K> * node, K val) {
  cout << " Running decreaseKey " << endl;

  node->key = val;

  while (true) {
    cout << " Loop counts " << endl;  
    fibnode<T, K> *adult = node->parent;

    if(adult == NULL) {                                   // no parent case
      node->mark = 0;                                    // cannot mark parents
      if (node->key < min->key) {min = node;}
      break;
    }

    else if(node->key > adult->key) {                     // heap is not violated
      break;
    }

    // heap violation cases
    else if(node->key < adult->key && !adult->mark) {    // adult not marked
    // cut out the node then mark adult (not roots)
      cut(node, adult);
      if (node->key < min->key) {min = node;}
      break;
    }

    else {
    // this case requires recursive cutting thus no break and set node to adult                         
      node = cut(node, adult);   
      if (node->key < min->key) {min = node;}
    }

  }
  
  
}

// node<T, K> * FibonnaciHeap<T, K>::decrease_key(const K& key) {
// }

template <typename T, typename K>
void FibonnaciHeap<T, K>::Consolidate() {



  /* THE MINIMUM SHOULD CURRENTLY BE AN ARBITRARY ROOT NODE */
  if(trees < 2) {return;}
  fibnode<T, K> *current = min;             // start at the arb. root node
  fibnode<T, K> *temp;                        // if identical degrees exist
  vector<fibnode <T,K> *> degTable;

  for(unsigned int i = 0; i < maxDeg+1; i++) {
    degTable.push_back(NULL);
  }

  unsigned int curTrees = trees;
  // consolidate the heap until all nodes before minimum element is satisfied
  for (unsigned i = 0; i < curTrees; i++) {
    // cout << "the ith loop in consolidate : " << i << endl;
    cout << current->key << " is key current loop i" << endl;

    if(current->key < min->key) {min = current;}

    if(degTable[current->deg] == NULL) {
      degTable[current->deg] = current;
      current = current->next;
    }

    else {
      temp = current;
      current = current->next;                                      // never vistied in root
      quickMeld(temp, degTable);
//      counter += loopReduction;
    }
  }
}

template <typename T, typename K>
void FibonnaciHeap<T,K>::quickMeld(fibnode<T,K> *target, vector<fibnode<T, K>*> &degTable) {
  fibnode<T, K> *root;
  fibnode<T, K> *kid;
  while(true) {
    // check relationship

    if (target->key < degTable[target->deg]->key) {
      root = target;
      kid = degTable[target->deg];
    }
    else {
      root = degTable[target->deg];
      kid = target;
    }

    if (min == kid) {min = root;}
    // pop out the kid
    (kid->next)->prev = kid->prev;
    (kid->prev)->next = kid->next;
    
    cout << " I am using " << root->key << " to childize " << kid->key << endl;


    // cases for making kid
    if(root->child == NULL) {
      cout << "bha" << endl;
      if (root->parent) {cout << "PROBLEM" << endl;}
      // info(root);
      root->child = kid;
      kid->parent = root;
      kid->next = kid;
      kid->prev = kid;
      // cout << "kid" << endl;
      // info(kid);
      // cout << "root" << endl;
      // info(root);
      if (root->parent) {cout << "PROBLEM" << endl;}
    }
    else {
      (root->child)->prev = kid;
      kid->next = root->child;

      for (unsigned int i = 0; i < root->deg-1; i++) {
        root->child = (root->child)->next;
      }

      (root->child)->next = kid;
      kid->prev = root->child;
      root->child = kid;
      kid->parent = root;
    }

    //maintain vars
    degTable[root->deg] = NULL;
    trees--;
    root->deg++;

    if (root->deg > maxDeg) {
      degTable.push_back(root);
      maxDeg = root->deg;
      break;
    }

    else if (degTable[root->deg] == NULL) {
      degTable[root->deg] = root;
      break;
    }
    
    else {
      target = root;
    }

  } 
  if (root->parent) {cout << "PROBLEM!" << endl;}
}


#endif
