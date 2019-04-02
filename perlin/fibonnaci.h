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

  int size() {return heapSize;}

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

  // Doesn't use recursion and instead loops elements passed in the meld using a vector 
  // of degrees.
  void quickMeld(fibnode<T, K> *current, vector<fibnode<T, K> *> &degTable);

  /*consolidates (unionizes and combines) the fibHeap
    consolidate should only be run after a minimum element is popped
    when the function is run the min of the fibHeap is not the true minimum
    and thus consolidate also finds the new minimum of the heap*/
  void slowConsolidate();
  void Consolidate();

  // Decrease the key of the node to value
  void decreaseKey(fibnode<T,K> * node, K val);

  // cut functions
  // cascade cut will return the adult of the node cut
  fibnode<T, K> * cut(fibnode<T, K> *node, fibnode<T, K> *adult);
  // fibnode<T, K> * cascade_cut(fibnode<T, K> *node);

private:
  unsigned int heapSize;    // number of nodes
  unsigned int maxDeg;      // maximum degree of nodes in heap
  unsigned int trees;       // number of trees (along root)
  fibnode<T, K> *start;     // start of loop for consolidate
  fibnode<T, K> *min;       // pointer to minimum key in fibHeap
  unsigned int marked;      // number of marked nodes //
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
void FibonnaciHeap<T, K>::slowConsolidate() {

  /* THE MINIMUM SHOULD CURRENTLY BE AN ARBITRARY ROOT NODE */
  start = min;                                // tracks node where to end consolidate
  fibnode<T, K> *current = start;             // start at the arb. root node
  fibnode<T, K> *foundNode;                   // if identical degrees exist
  // unordered map storing degrees, and their root node //
  unordered_map<unsigned int, fibnode<T,K>* > roots; 

  
  // consolidate the heap until all nodes before minimum element is satisfied
  do {
    auto iter = roots.find(current->deg);     // find root degrees in fibHeap are identical to current
    if (iter == roots.end()) {                // if it doesn't exist put into roots
      roots[current->deg] = current;
    }

    else if (iter->second == current) {
      trees--;
      foundNode = iter->second;                               // get the identical node
      roots.erase(iter);                                      // erase the found 
      current = recursiveMeld(foundNode, current, roots);     // meld the nodes in question
    }

    if (current->key < min->key) {min = current;}
    current = current->next;


  } while (current != start);

}

template <typename T, typename K>
void FibonnaciHeap<T, K>::popMin() {
  // cannot popMin of empty heap
  assert(heapSize != 0);
  cout << "MIN KEY before pop :" << min->key << endl;
  cout << "MIN ITEM before pop :" << min->item << endl;
  cout << "next min item:" << min->next->item << endl;

  fibnode<T, K> *poppedMin = min; 

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
    cout << "okey";
    trees--;
                for(unsigned int i = 0; i < trees; i++) {
        if(min->parent != NULL) {cout << "okey kids" << endl;}
        min = min->next;
  }

  }

  else if (trees == 1) { // single tree only
    if (min->deg == maxDeg) {maxDeg = min->deg;}
    fibnode<T, K> *current = min->child;
    current->parent = NULL;

    for(unsigned int i = 0; i < min->deg-1; i++) {
      current = current->next;
      current->parent = NULL;
    }

    trees = min->deg;
    min = current;

    cout << "dokey";

            for(unsigned int i = 0; i < trees; i++) {
        if(min->parent != NULL) {cout << "dokeys kids" << endl;}
        min = min->next;
  }

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
    min->prev->next = current;


    for(unsigned int i = 0; i < min->deg-1; i++) {
      current = current->next;
      current->parent = NULL;
    }

    current->next = min->next;
    min->next->prev = current;

    // (min->next)->prev = current;
    // current->next = min->next;    
    trees += min->deg-1;
    // set arbritrary minimum as last element of the children
    min = current;
        cout << "artichoke";

            for(unsigned int i = 0; i < trees; i++) {
        if(min->parent != NULL) {cout << "artichokes kids" << endl;}
        min = min->next;
  }


  }
  cout << "AFTER POP MIN before consolidate0 :   " << min->prev->item << endl;

  cout << "AFTER POP MIN before consolidate1 :   " << min->item << endl;
  cout << "AFTER POP MIN before consolidate2 :   " << min->next->item << endl;
  cout << "AFTER POP MIN before consolidate3s :   " << min->next->next->item << endl;

    for(unsigned int i = 0; i < trees; i++) {
        if(min->parent != NULL) {cout << "dont make kids popmin" << endl;}
        min = min->next;
  }

  delete poppedMin;
  heapSize--;




  // minimum has been popped so find new minimum and satisfy fib property
  Consolidate();
}

template <typename T, typename K>
fibnode<T, K> * FibonnaciHeap<T, K>::cut(fibnode<T, K> *node, fibnode<T, K> *adult) {
  cout << "entered cut" << endl;


  node->parent = NULL;
  trees++;

  // mark the cut unless it's a root 
  if (adult->parent != NULL) {
    adult->mark = true;       
  }
  node->mark = false;

  if (adult->deg == 1) {                 // the node cut is the only node
    adult->child = NULL;
  }

  else if (adult->child == node) {      // disown the kid
    adult->child = node->next;
    (node->next)->prev = node->prev;
    (node->prev)->next = node->next;
  }

  else {
    (node->next)->prev = node->prev;
    (node->prev)->next = node->next;
  }


  cout << "rooting : " << node->item << endl;

  // pop out the node and put into roots

  (min->next)->prev = node;
  node->next = min->next;
  node->prev = min;
  min->next = node;

  adult->deg--;
}

template <typename T, typename K>
void FibonnaciHeap<T, K>::decreaseKey(fibnode<T,K> *node, K val) {
  cout << " Running decreaseKey " << endl;

  node->key = val;
  fibnode<T, K> *temp = node;

  while (true) {
    fibnode<T, K> *adult = temp->parent;

    if(adult == NULL) {                                   // no parent case
      temp->mark = 0;                                    // cannot mark parents
      break;
    }

    else if(temp->key > adult->key) {                     // heap is not violated
      break;
    }

    // heap violation cases
    else if(!adult->mark) {    // adult not marked
    // cut out the temp then mark adult (not roots)
        cout << "cancercut" << endl;

      cut(temp, adult);
        cout << "cancercut" << endl;

      break;
    }

    else {
    // this case requires recursive cutting thus no break and set temp to adult            
      cout << "cancer" << endl;
      
      fibnode<T, K> *temp2 = temp;
      cut(temp, adult);   
      temp = adult;

        cout << "cancer" << endl;

    }

  }
  if (node->key < min->key) {min = node;}

  cout << "afterDECMIN ITEM :" << min->item << endl;
  cout << "this is next after dec1" << min->next->item << endl;
  cout << "this is next after dec2" << min->next->next->item << endl;

  cout << "this is next after dec3" << min->next->next->next->item << endl;

  cout << "this is next after dec4" << min->next->next->next->next->item << endl;

  cout << "this is next after dec5" << min->next->next->next->next->next->item << endl;


  
}

// node<T, K> * FibonnaciHeap<T, K>::decrease_key(const K& key) {
// }

template <typename T, typename K>
void FibonnaciHeap<T, K>::Consolidate() {
  cout << "Trees yo : " << trees << endl;


  /* THE MINIMUM SHOULD CURRENTLY BE AN ARBITRARY ROOT NODE */
  if(trees == 1) {return;}
  fibnode<T, K> *current = min;             // start at the arb. root node
  fibnode<T, K> *temp;                        // if identical degrees exist
  vector<fibnode <T,K> *> degTable;

  for(unsigned int i = 0; i < maxDeg+1; i++) {
    degTable.push_back(NULL);
  }

  unsigned int curTrees = trees;
  // consolidate the heap until all nodes before minimum element is satisfied
  for (unsigned i = 0; i < curTrees; i++) {
    cout << "TREE RIGHT NOW : " << trees << endl;

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

    if(current->key < min->key) {min = current;}
  for(unsigned int i = 0; i < trees; i++) {
        if(current->parent != NULL) {cout << "weezy WARNING !!!!!!!!!!!" << endl;}
        current = min->next;
  }
  }

  for(unsigned int i = 0; i < trees; i++) {
        if(current->parent != NULL) {cout << "weezy WARNING !!!!!!!!!!!" << endl;}
        current = min->next;
  }

  cout << trees << " tree yike" << endl;
  cout << " AFTER CONSOLIDATE MINIMUM : " << min->item << endl;
    cout << " AFTER CONSOLIDATE MINIMUM2 : " << min->next->item << endl;

}

template <typename T, typename K>
void FibonnaciHeap<T,K>::quickMeld(fibnode<T,K> *target, vector<fibnode<T, K>*> &degTable) {
  // cout << trees << "baby time" << endl;
  cout << "quickmelding" << endl;
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

          fibnode<T, K> *what = root;

    for (int i = 0; i < trees; i++) {
    if(what->parent != NULL) {cout << "i was wrong help" << endl;}
    what = what->next;
}
    // pop out the kid
    (kid->next)->prev = kid->prev;
    (kid->prev)->next = kid->next;
    
    // cases for making kid
    if(root->child == NULL) {
      root->child = kid;
      kid->parent = root;
      kid->next = kid;
      kid->prev = kid;
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
      degTable.push_back(NULL);
      degTable[root->deg] = root;
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
}


#endif
