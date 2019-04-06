#ifndef _ASTAR_H_
#define _ASTAR_H_
#include "heap.h"
#include <iostream>
#include <limits>
#include <cmath>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "fibonnaci.h"
#include "dijkstra.h"
#include "wdigraph.h"
#include "server_util.h"
using namespace std;

typedef pair<int, int> PII;

  struct node {
    node* parent = nullptr;
    bool visited = false;
    int x;
    int y;
    int id;
    // h is estimated cost from node to end node, 
    // g is distance from source node to new node
    // f is the total cost from source to goal
    float f, g, h;    
    vector<node*> neighbours;
};

float distance(node *a, node *b) {
    return abs(a->x - b->x) + abs(a->y - b->y);
}

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



void astar(int start, int end, const unordered_map<pointData, pair<int,int>, pointHash> vertex, WDigraph graph) {
    node p;
    node *point = &p;
    unordered_map<int, node*> all;
    for (auto i = vertex.begin(); i != vertex.end(); ++i) {
        point->x = i->first.x;
        point->y = i->first.y;
        point->id = i->second.first;

        point->f = numeric_limits<float>::max();
        point->g = numeric_limits<float>::max();
        all[i->second.first]= point;
    }
    cout << all[start]->id << " " << all[start]->x << " " << all[start]->y << endl;
    all[start]->f = distance(all[start],all[end]);
    all[start]->g = 0;
    node *currentNode = all[start];

    BinaryHeap<float, node*> open;  // nodes to be traversed
    // BinaryHeap<float, node*> closed; // nodes already traversed
    // BinaryHeap<float, node*> closed;

    open.insert(all[start]->g, all[start]);

    while (open.size() != 0) {
        while (open.size()!=0 && open.min().second->visited) {
            open.popMin();
        }
        if (open.size() == 0) {break;}
        currentNode = open.min().second;
        currentNode->visited = true;
        for (auto i = graph.neighbours(currentNode->id); i!= graph.endIterator(currentNode->id); ++i) {
            if (!all[*i]->visited) {
                open.insert(all[*i]->g, all[*i]);
            }

            float cost = graph.getCost(currentNode->id, *i) + distance(all[currentNode->id], all[*i]);
            if (cost < all[*i]->g) {
                all[*i]->parent = currentNode;
                all[*i]->g = cost;
                all[*i]->h = all[*i]->g + distance(all[*i], all[end]);
            }
        }
    }
    cout << all[start]->id << " " << all[start]->x << " " << all[start]->y << endl;
}


// void dijkstra(const WDigraph& graph, int startVertex, 
//   unordered_map<int, PLI>& tree) {
// // this function produces a search tree from a given weighted digraph and the startvertex of 
// // the desired search. 

//   BinaryHeap<PII, long long> fires;               // uses the concept of burning "fires"
//   fires.insert(PII(startVertex, -1), 0);          // inserts start vertex with no predeccessor at time 0

//   while(fires.size() > 0) {                       // fires exists

//     // obtain earliest fire or heap that is about to expire
//     auto earliestFire = fires.min();
//     int v = earliestFire.first.first, u = earliestFire.first.second;
//     long long time = earliestFire.second;

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
#endif