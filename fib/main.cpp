#include "fibonnaci.h"
#include <iostream>

using namespace std;

int main() {
  FibonnaciHeap<int, int> heap;
  fibnode <int, int> *hi = heap.insert(5, 5);
  fibnode <int, int> *hi2 = heap.insert(10, 10);
  fibnode <int, int> *hi3 = heap.insert(15, 15);


  cout << hi->key << " : " << hi->item << endl;
  cout << hi2->key << " : " << hi2->item << endl;
  cout << "Der: " << hi2->next->key << endl;
  cout << hi3->key << " : " << hi3->item << endl;
  cout << endl;

  cout << hi->next->key << endl;
  cout << hi2->next->key << endl;
  cout << hi3->next->key << endl;


  return 0;
}