#include "fibonnaci.h"
#include <iostream>

using namespace std;

int main() {
  FibonnaciHeap<int, int> heap;
  fibnode <int, int> *hi = heap.insert(5, 5);
  cout << hi->key << endl;
  return 0;
}