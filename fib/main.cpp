#include "fibonnaci.h"
#include <iostream>
#include "heap.h"
#include <time.h>

using namespace std;

void potato(int& var) {
  var = var - 5;
}

int main() {
  FibonnaciHeap<int, int> heap;
  BinaryHeap<int, int> bheap;
  clock_t start, end;
  double cpu_time_used;

  fibnode <int, int> *hi1 = heap.insert(1, 1);
  fibnode <int, int> *hi2 = heap.insert(2, 2);
  fibnode <int, int> *hi3 = heap.insert(3, 3);
  // fibnode <int, int> *hi4 = heap.insert(4, 4);
  // fibnode <int, int> *hi5 = heap.insert(5, 5);
  // fibnode <int, int> *hi6 = heap.insert(6, 6);
  // fibnode <int, int> *hi7 = heap.insert(7, 7);
  // fibnode <int, int> *hi8 = heap.insert(8, 8);
  // fibnode <int, int> *hi9 = heap.insert(9, 9);

  start = clock();
  for(int i = 4; i < 6456789; i++) {
    heap.insert(i, i);
    //bheap.insert(i,i);
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "fib time: " << cpu_time_used << endl;

  start = clock();
  for(int i = 1; i < 6456789; i++) {
    //heap.insert(i%239, i%16);
    bheap.insert(i,i);
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "bih time: " << cpu_time_used << endl;


  cout << hi1->key << " : " << hi1->item << endl;
  cout << hi2->key << " : " << hi2->item << endl;
  cout << hi3->key << " : " << hi3->item << endl;
  cout << endl;

  cout << "      minCHECK" << endl;
  cout << heap.getMin().key << endl;
  cout << "      minCHECK" << endl;

  // int var = 5;
  // potato(var);
  // cout << var << endl;

  cout << "POPCHECK" << endl;
  // heap.popMin();
  // heap.popMin();
  heap.popMin();
  //cout << "ran";
  cout << heap.getMin().key << endl;
  cout << "POPCHECK" << endl;

  cout << "ROOTCHECK" << endl; // for 5 roots not including minimum
  cout << heap.getMin().prev->key << endl;
  cout << heap.getMin().next->key << endl;
  cout << heap.getMin().next->next->key << endl;
  cout << heap.getMin().next->next->next->key << endl;
  cout << heap.getMin().next->next->next->next->key << endl;
  cout << heap.getMin().next->next->next->next->next->key << endl;
  cout << "ROOTCHECK" << endl;

  cout << "      minCHECK" << endl;
  cout << heap.getMin().key << endl;
  cout << "      minCHECK" << endl;


  // cout << "KIDCHECK" << endl;
  // cout << heap.getMin().child->key << endl;
  // cout << "KIDCHECK" << endl;

  cout << "      minCHECK" << endl;
  cout << heap.getMin().key << endl;
  cout << "      minCHECK" << endl;

  start = clock();
  for (int i = 0; i < 5000; i++) {
    heap.popMin();
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "fib time: " << cpu_time_used << endl;



  // start = clock();
  // for (int i = 0; i < 5000; i++) {
  //   bheap.popMin();
  // }

  // end = clock();
  // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  // cout << "bih time: " << cpu_time_used << endl;
  return 0;
}