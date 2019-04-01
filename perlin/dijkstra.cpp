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
#include <iostream>

using namespace std;

typedef pair<long long, int> PLI;
typedef pair<int, int> PII;

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