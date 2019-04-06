#ifndef _ASTAR_H_
#define _ASTAR_H_

#include <cmath>
#include <unordered_map>
#include "fibonnaci.h"
#include "dijkstra.h"
#include "wdigraph.h"
#include "pointData.h"
using namespace std;

typedef pair<int, int> PII;

float calcDistance(int x1, int y1, int x2, int y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}

void fibstar(int start, int end, const unordered_map<pointData, pair<int,int>, pointHash> vertex, const WDigraph &graph, unordered_map<int, PLI>& tree, const pointData startCoords, const pointData endCoords) {

  long long INFINITE = 9223372036854775807;
  long long duration = 0;
  FibonnaciHeap<PII, long long> fires;
  long long x1, y1;
  long long disheuristic = 0;
  // unordered_map<int, fibnode<int, long long> *> vertexMap;

  // initialize vector containing all fibnodes

  vector< pair<long long, fibnode<PII, long long>* > > vertexMap(vertex.size(), pair<long long, fibnode<PII, long long> *>(0, NULL));

  // insert all vertexes into heap and give them indeterminate (-1) predecessor vectors
  for (auto i = vertex.begin(); i != vertex.end(); ++i) {
    x1 = i->first.x;
    y1 = i->first.y;
    // use heuristic of manhatten distance between each node and endnode
    disheuristic = calcDistance(x1, y1, endCoords.x, endCoords.y);

    fibnode<PII, long long> *addedNode = fires.insert(PII(i->second.first, -1), INFINITE);
    vertexMap[i->second.first] = pair<long long, fibnode<PII, long long> * >(disheuristic, addedNode);
  }

  fires.decreaseKey(vertexMap[start].second, duration + disheuristic);

  while(fires.size() > 0) {

    fibnode<PII, long long> earliestFire = fires.getMin();

    int v = earliestFire.item.first; 
    int u = earliestFire.item.second;
    duration = earliestFire.key;

    fires.popMin();

    tree[v] = PLI(duration, u);
  //    prev = v;

    if(v == end) {return;}
    // loop all neighbours and add them to the list of burning fires if they are not present
    for(auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {

      int nbr = *iter;
      if(tree.find(nbr) == tree.end()) {
        long long burn = duration + graph.getCost(v, nbr) + vertexMap[nbr].first;     // burn weight is the movement cost combined with distance heurisitic

        if (vertexMap[nbr].second->key > burn) {
          fires.decreaseKey(vertexMap[nbr].second, burn);
          vertexMap[nbr].second->item.second = v;
        }
      }
    }
  }
}

#endif