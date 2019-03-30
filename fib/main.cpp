#include "fibonnaci.h"
#include <iostream>

using namespace std;

void potato(int& var) {
  var = var - 5;
}

int main() {
  FibonnaciHeap<int, int> heap;
  fibnode <int, int> *hi = heap.insert(5, 5);
  fibnode <int, int> *hi2 = heap.insert(10, 10);
  fibnode <int, int> *hi3 = heap.insert(15, 15);


  cout << hi->key << " : " << hi->item << endl;
  cout << hi2->key << " : " << hi2->item << endl;
  cout << hi3->key << " : " << hi3->item << endl;
  cout << endl;

  cout << hi->next->key << endl;
  cout << hi2->next->key << endl;
  cout << hi3->next->key << endl;

  cout << "minCHECK" << endl;
  // cout << heap.getMin().key << endl;
  //heap.popMin();
  cout << heap.getMin().key << endl;
  cout << "minChecked" << endl;

  // int var = 5;
  // potato(var);
  // cout << var << endl;

  cout << "money" << endl;
  heap.popMin();
  cout << heap.getMin().key << endl;
  cout << heap.getMin().next->child->key << endl;


  return 0;
}