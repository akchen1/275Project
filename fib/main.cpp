#include "fibonnaci.h"
#include <iostream>
#include "heap.h"
#include <time.h>
#include <unordered_map>

using namespace std;

int main() {
  FibonnaciHeap<int, int> heap;
  
  unordered_map<int, fibnode<int, int> *> vertex_map;
char type;
int key, val;
  while (true) {
    
    cin >> type;
    if (type == 'I') {
      cin >> key >> val;
      vertex_map[val] = heap.insert(key,val);
    }
    else if (type == 'P') {
      heap.popMin();
    }
    else if (type == 'M') {
      heap.getMin();
    }
    else if (type == 'D') {
      cin >> val;
      heap.decreaseKey(vertex_map[val], 0);
    }
  }
  return 0;
  // BHEAP POPMIN SPEED

}