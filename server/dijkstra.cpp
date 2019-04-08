/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  275 Project: Dijkstra.cpp

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

using namespace std;

typedef pair<long long, int> PLI;
typedef pair<int, int> PII;
typedef pair<long long, long long> PLL;

void fibdijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PLI>& tree, list<int> &vertex) {
// this function uses fibonnaci heaps with dijkstras to increase the running time

  long long INFINITE = 9223372036754775806;
  long long duration = 0;
  FibonnaciHeap<PII, long long> fires;
  // initialize variables

  // initialize vector containing all fibnodes
  vector<fibnode<PII, long long> *> vertexMap(vertex.size(), NULL);

  // insert all vertexes into heap and give them indeterminate (-1) predecessor vectors
  // they all have time infinite since the path is undetermined
  for(auto iter : vertex) {
    vertexMap[iter] = fires.insert(PII(iter, -1), INFINITE);
  }

  // decrease the start vertex to time 0
  fires.decreaseKey(vertexMap[startVertex], duration);

  // visit all elements in the searchTree
  while(fires.size() > 0) {

    // get the min
    fibnode<PII, long long> earliestFire = fires.getMin();

    int v = earliestFire.item.first; 
    int u = earliestFire.item.second;
    duration = earliestFire.key;

    fires.popMin();
    // register vertex in the search tree
    tree[v] = PLI(duration, u);

    // loop all neighbours and add them to the list of burning fires if they are not present
    for(auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {

      int nbr = *iter;
      if(tree.find(nbr) == tree.end()) {
        long long burn = duration + graph.getCost(v, nbr);
        // only add the neighbour if the burn value is less than the current
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
// the desired search using a binary heap

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