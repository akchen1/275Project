/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  Assignment 2: Heap.h

  Program Description:
  This file is build off of a file provided in eClass from the CMPUT 275 
  called "heap.h". It holds a template class BinaryHeap which has various 
  functions that can be used to manipulate the heap.
*/


#ifndef _HEAP_H_
#define _HEAP_H_

#include <vector>
#include <utility> // for pair'
#include <iostream>

// T is the type of the item to be held
// K is the type of the key associated with each item in the heap
// The only requirement is that K is totally ordered and comparable via <
template <class T, class K>
class BinaryHeap {
public:
  // constructor not required because the only "initialization"
  // is through the constructor of the variable "heap" which is called by default

  // return the minimum element in the heap
  std::pair<T, K> min() const {
    return heap[0];
  }

  // insert an item with the given key
  // if the item is already in the heap, will still insert a new copy with this key

  void insert(const T& item, const K& key) {
    std::pair<T, K> v = make_pair(item, key);                     // generate pair v from item/key
    std::pair<T, K> temp;
    heap.push_back(v);                                            // insert v to heap
    int idx = heap.size()-1;                                      // size uses 1-based indexing, there index is one less

    while (v != heap[0] && v.second < heap[(idx-1)/2].second) {   // loop if parent is greater than child
      // swap parent and child then set index to parent
      temp = heap[(idx-1)/2];
      heap[(idx-1)/2] = v;
      heap[idx] = temp;
      int idx = (idx-1)/2;
    }
  }

  // pop the minimum item from the heap
  void popMin() {
    if(heap.size() == 0) {return;}                          // nothing to check
    heap[0] = heap[heap.size() - 1];                        // set last element to heap 0;       
    heap.pop_back();                                        // remove last element

    // variables
    int idx = 0;                          // current index
    int min = 0;                          // minimum key child
    int left;                             // left child
    int right;                            // right child
    int size = heap.size() - 1;

    while(true) {                         // while loop only breaks if there are no children left
      left = 1*idx + 1;
      right = 2*idx + 2;

      // conditions for when there are 1 or no children
      if(left > size) {break;}                      
      else if (right > size) {min = left;}        // only left exists

      // find if left or right is bigger key and store in min
      else {
        if (heap[right].second > heap[left].second) {
          min = left;
        }        
        else {min = right;}
      }

      // swap parent/child and set the index to current minimum
      auto u = heap[min];
      heap[min] = heap[idx];
      heap[idx] = u;
      idx = min;

    }
  }

  // return the size of the heap
  int size() const {return heap.size();}

private:
  // the array holding the heap
  std::vector< std::pair<T, K> > heap;

  // feel free to add appropriate private methods to help implement some functions

  
};



#endif
