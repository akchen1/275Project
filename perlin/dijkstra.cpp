/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  Assignment 2: Dijkstra.cpp

  Program Description:
  This file is build off of a file provided in eClass from the CMPUT 275 
  called "dijkstra.cpp" it uses the same concept, however it has been implemented
  using a binaryHeap.
*/

#include "dijkstra.h" 
#include "fibonnaci.h"
#include "heap.h"
#include <iostream>
#include <unordered_map>
//#include "Server_main.cpp"

using namespace std;

typedef pair<long long, int> PLI;
typedef pair<int, int> PII;

void fibdijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PLI>& tree, list<int> &vertex) {

  long long INFINITE = 9223372036754775806;
  long long duration = 0;
  FibonnaciHeap<PII, long long> fires;
  // unordered_map<int, fibnode<int, long long> *> vertexMap;

  // initialize vector containing all fibnodes

  vector<fibnode<PII, long long> *> vertexMap(vertex.size(), NULL);

  // insert all vertexes into heap and give them indeterminate (-1) predecessor vectors
  for(auto iter : vertex) {
    vertexMap[iter] = fires.insert(PII(iter, -1), INFINITE);
  }

  fires.decreaseKey(vertexMap[startVertex], duration);

  while(fires.size() > 0) {

    fibnode<PII, long long> earliestFire = fires.getMin();

    int v = earliestFire.item.first; 
    int u = earliestFire.item.second;
    duration = earliestFire.key;

    fires.popMin();

    tree[v] = PLI(duration, u);
  //    prev = v;

    // loop all neighbours and add them to the list of burning fires if they are not present
    for(auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {

      int nbr = *iter;
      if(tree.find(nbr) == tree.end()) {
        long long burn = duration + graph.getCost(v, nbr);

        if (vertexMap[nbr]->key > burn) {
          fires.decreaseKey(vertexMap[nbr], burn);
          vertexMap[nbr]->item.second = v;
        }
      }
    }
  }
}

void bdijkstra(const WDigraph& graph, int startVertex, 
  unordered_map<int, PLI>& tree) {
// this function produces a search tree from a given weighted digraph and the startvertex of 
// the desired search. 

  BinaryHeap<PII, long long> fires;               // uses the concept of burning "fires"
  fires.insert(PII(startVertex, -1), 0);          // inserts start vertex with no predeccessor at time 0

  while(fires.size() > 0) {                       // fires exists

    // obtain earliest fire or heap that is about to expire
    auto earliestFire = fires.min();
    int v = earliestFire.first.first, u = earliestFire.first.second;
    long long time = earliestFire.second;

    // pop out the earliest fire
    fires.popMin();

    // vertex hasn't been burned in the tree map
    if(tree.find(v) == tree.end()) {
      // use vertex key to add burning time and predecessor
      tree[v] = PLI(time, u);

      // loop all neighbours and add them to the list of burning fires
      for(auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
        int nbr = *iter;
        long long burn = time + graph.getCost(v, nbr);
        fires.insert(PII(nbr, v), burn);
        
      }
    }
  }
}

// void dijkstra(const WDigraph& graph, int startVertex, 
//   unordered_map<int, PLI>& tree) {
// // this function produces a search tree from a given weighted digraph and the startvertex of 
// // the desired search. 

//   FibonnaciHeap<PII, long long> fires;               // uses the concept of burning "fires"
//   fires.insert(PII(startVertex, -1), 0);          // inserts start vertex with no predeccessor at time 0

//   while(fires.size() > 0) {                       // fires exists

//     // obtain earliest fire or heap that is about to expire
//     auto earliestFire = fires.getMin();
//     int v = earliestFire.item.first, u = earliestFire.item.second;
//     long long time = earliestFire.key;

//     // pop out the earliest fire
//     fires.popMin();

//     // vertex hasn't been burned in the tree map
//     if(tree.find(v) == tree.end()) {
//       // use vertex key to add burning time and predecessor
//       tree[v] = PLI(time, u);

//       // loop all neighbours and add them to the list of burning fires
//       for(auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
//         int nbr = *iter;
//         long long burn = time + graph.getCost(v, nbr);
//         fires.insert(PII(nbr, v), burn);
        
//       }
//     }
//   }
// }

void dijkstra(const WDigraph& graph, int startVertex, 
  unordered_map<int, PLI>& tree) {
// this function produces a search tree from a given weighted digraph and the startvertex of 
// the desired search. 

  BinaryHeap<PII, long long> fires;               // uses the concept of burning "fires"
  fires.insert(PII(startVertex, -1), 0);          // inserts start vertex with no predeccessor at time 0

  while(fires.size() > 0) {                       // fires exists

    // obtain earliest fire or heap that is about to expire
    auto earliestFire = fires.min();
    int v = earliestFire.first.first, u = earliestFire.first.second;
    long long time = earliestFire.second;

    // pop out the earliest fire
    fires.popMin();

    // vertex hasn't been burned in the tree map
    if(tree.find(v) == tree.end()) {
      // use vertex key to add burning time and predecessor
      tree[v] = PLI(time, u);

      // loop all neighbours and add them to the list of burning fires
      for(auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
        int nbr = *iter;
        long long burn = time + graph.getCost(v, nbr);
        fires.insert(PII(nbr, v), burn);
        
      }
    }
  }
}