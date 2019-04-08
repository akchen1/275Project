/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  Assignment 2: fibonnaci.h
  This file allows the user to create a fibonnaci heap, it contains methods that allow the user to manipulate a fib heap
*/
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
// fibnodes are nodes within a fibonnaci tree
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
  FibonnaciHeap() {   // intialize the heap;
    heapSize = maxDeg = trees = marked = 0;
    min = NULL;
  }

  // deconstructor (just pop all minimum out)
  ~FibonnaciHeap() {
    while (heapSize > 0) {
      popMin();
    }
  }

  // return minimum of heap
  fibnode<T, K> getMin(); 

  // insert a single tree into the FibHeap and return the pointer to the node added to heap
  /* *note* user should maintain their own vector of pointers to nodes to ensure the ability to decrease
     key of a given node */
  fibnode<T, K> * insert(const T& item, const K& key);

  // removes the minimum from fibheap
  void popMin();

  // makes two nodes consolidate into a single tree if they have the same degree
  void quickMeld(fibnode<T, K> *current, vector<fibnode<T, K> *> &degTable);

  /*consolidates (unionizes and combines) the fibHeap
    consolidate should only be run after a minimum element is popped
    when the function is run the min of the fibHeap is not the true minimum
    and thus consolidate also finds the new minimum of the heap*/
  void Consolidate();

  // return heapsize
  unsigned int size();

  // Decrease the key of the node to value
  void decreaseKey(fibnode<T,K> * node, K val);

  // cut functions
  // cascade cut will return the adult of the node cut
  fibnode<T, K> * cut(fibnode<T, K> *node, fibnode<T, K> *adult);
  fibnode<T, K> * cascade_cut(fibnode<T, K> *node, fibnode<T, K> *adult);

  // debugging returns information of a node
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
unsigned int FibonnaciHeap<T,K>::size() {
  return heapSize;
}

template <typename T, typename K>
void FibonnaciHeap<T,K>::info(const fibnode<T,K>  *current_node) {
  // cout << current_node->key << " at location "<< &current_node << endl;
  // cout << "degree of : " << current_node->deg << endl;
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
  assert(heapSize > 0);
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
  return current_node;
}


template <typename T, typename K>
void FibonnaciHeap<T, K>::popMin() {
  /* before consolidate is run, the min is set to any node on the root list */
  // cannot popMin of empty heap
  assert(heapSize != 0);

  fibnode<T, K> *poppedMin = min; 
  if(heapSize == 1) { // if there is only a single element

    delete min; 
    heapSize--;
    trees--;
    return;
  }

  else if(min->child == NULL) { // if the min has no children then simply pop it out of the roots
    (min->prev)->next = min->next;
    (min->next)->prev = min->prev;
    min = min->next;
  }

  else if (trees == 1) { // single tree only
    if (min->deg == maxDeg) {maxDeg--;}
    fibnode<T, K> *current = min->child;
    current->parent = NULL;
    current->mark = false;

    for(unsigned int i = 0; i < min->deg-1; i ++) { // all children must be changed to root nodes
      current = current->next;
      current->parent = NULL;
      current->mark = false;
    }

    trees += min->deg;
    min = current;
  }

  // popped minimum has children then push children to roots
  else {
    // keep track of node
    fibnode<T, K> *current = min->child;
    if (min->deg == maxDeg) {maxDeg--;}

    // for all children move to the roots
    current->parent = NULL;
    current->mark = false;
    current->prev = min->prev;
    current->prev->next = current;

    for(unsigned int i = 0; i < min->deg-1; i++) {
      current = current->next;
      current->parent = NULL;
      current->mark = false;
    }

    current->next = min->next;
    current->next->prev = current;

    trees += min->deg;
    // set arbritrary minimum as last element of the children
    min = current;
  }

  // delete minimum
  trees--;
  delete poppedMin;
  heapSize--;

  Consolidate();  // consolidate the remaining tree
}

template <typename T, typename K>
fibnode<T, K> * FibonnaciHeap<T, K>::cut(fibnode<T, K> *node, fibnode<T, K> *adult) {

  // one more tree produced
  trees++;

  // mark the cut unless it's a root 
  if (adult->parent != NULL) {
    adult->mark = true;       
  }
  node->mark = false;

  if (adult->deg == 1) {                 // the node cut is the only node
    adult->child = NULL;
  }

  else if (adult->child == node) {      // disown the kid by transfering kids
    adult->child = node->next;
  }

  node->parent = NULL;

  // pop out the node and put into roots
  (node->next)->prev = node->prev;
  (node->prev)->next = node->next;

  // put in front of the minimum before recalculated
  (min->next)->prev = node;
  node->next = min->next;
  node->prev = min;
  min->next = node;

  adult->deg--; // cut from tree therefore the parent deg reduced

  return adult;
}


template <typename T, typename K>
fibnode<T, K> * FibonnaciHeap<T, K>::cascade_cut(fibnode<T, K> *node, fibnode<T, K> *adult) {
// cut is cascading so first cut the node from adult
  
  fibnode<T, K> *current_toCut = node;
  fibnode<T, K> *mother = adult;
  // fibnode<T, K> *temp;

  while (true) { // cut until the parent is not marked or on roots

    current_toCut = cut(current_toCut, mother);
    if(current_toCut->mark) {
      mother = current_toCut->parent;
    }
    else {break;}

  }
  return mother;
}


template <typename T, typename K>
void FibonnaciHeap<T, K>::decreaseKey(fibnode<T,K> * node, K val) {

  node->key = val;  // change nde to value

  fibnode<T, K> *adult = node->parent;

  // check the cases 
  if(adult == NULL) {                                   // no parent case
    node->mark = 0;                                    // cannot mark parents
    if (node->key < min->key) {min = node;}
    return;
  }

  else if(node->key > adult->key) {                     // heap is not violated
    return;
  }

  // heap violation cases
  else if(node->key < adult->key && !adult->mark) {    // adult not marked
  // cut out the node then mark adult (not roots)
    cut(node, adult);
    if (node->key < min->key) {min = node;}
  }

  else {
  // this case requires recursive cutting thus no break and set node to adult   
    cascade_cut(node, adult);
    if (node->key < min->key) {min = node;}
  }

}

template <typename T, typename K>
void FibonnaciHeap<T, K>::Consolidate() {

  /* THE MINIMUM SHOULD CURRENTLY BE AN ARBITRARY ROOT NODE */
  if(trees < 2) {return;}
  fibnode<T, K> *current = min;             // start at the arb. root node
  fibnode<T, K> *temp;                      // if identical degrees exist
  vector<fibnode <T,K> *> degTable;         // table tracking the degrees of the roots

  for(unsigned int i = 0; i < maxDeg+1; i++) {  // create a vector keeping track of degrees in the roots
    degTable.push_back(NULL);
  }

  unsigned int curTrees = trees;
  // consolidate the heap until all nodes before minimum element is satisfied
  for (unsigned i = 0; i < curTrees; i++) { // iterate through all root nodes

    if(current->key < min->key) {min = current;}  // check if new min found in roots

    if(degTable[current->deg] == NULL) {  // there is no matching degree case
      degTable[current->deg] = current;
      current = current->next;
    }

    else {  // matching degree case
      temp = current;
      current = current->next;                   // shift to next vistied in root
      quickMeld(temp, degTable);                 // meld the nodes that have same degree
    }
  }
}

template <typename T, typename K>
void FibonnaciHeap<T,K>::quickMeld(fibnode<T,K> *target, vector<fibnode<T, K>*> &degTable) {
  fibnode<T, K> *root;
  fibnode<T, K> *kid;
  while(true) { // keep melding until no degree copies are present
    // check relationship

    if (target->key < degTable[target->deg]->key) { // check which keys are smaller then set which is root and which is parent
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

    // cases for making kid
    if(root->child == NULL) { // no current kids

      root->child = kid;
      kid->parent = root;
      kid->next = kid;
      kid->prev = kid;

    }

    else {  // kids exist (make sure last and first element circle loop to the added node)
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

    if (root->deg > maxDeg) { // if the root is now a greater degree add to table
      degTable.push_back(root);
      maxDeg = root->deg;
      break;
    }

    else if (degTable[root->deg] == NULL) { // if new root node degree doesn't exist 
      degTable[root->deg] = root;
      break;
    }
    
    else {  // loop the meld again
      target = root;
    }

  } 
}


#endif
