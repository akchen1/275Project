#ifndef _ASTAR_H_
#define _ASTAR_H_
#include "heap.h"
#include <iostream>
#include <limits>
#include <cmath>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "wdigraph.h"
#include "server_util.h"
using namespace std;


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