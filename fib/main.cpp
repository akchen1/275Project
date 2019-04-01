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



  start = clock();
  // this commented bit is the 20 nodes crash failure 
  // for(int i = 1; i < 21; i++) {
  //   heap.insert(i, i);
  //   //bheap.insert(i,i);
  // }
  for(int i = 1; i < 6000; i++) {
    heap.insert(i, i);
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "fib insert time: " << cpu_time_used << endl;

  start = clock();
  for(int i = 1; i < 6000; i++) {
    //heap.insert(i%239, i%16);
    bheap.insert(i,i);
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "bih insert time: " << cpu_time_used << endl;


  start = clock();
  for (int i = 1; i < 6000; i++) {
   // cout << i << endl;
    cout << heap.getMin().key << endl;
    heap.popMin();
  }
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "fib time: " << cpu_time_used << endl;



  start = clock();
  for (int i = 0; i < 10; i++) {
    bheap.popMin();
  }

  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  cout << "bih time: " << cpu_time_used << endl;
  return 0;
}