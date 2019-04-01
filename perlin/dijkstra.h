/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  Assignment 2: dijkstra.h

  Program Description:
  This is a header file for dijkstra.cpp
*/

#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "heap.h"
#include "wdigraph.h"
#include <list>
#include <utility>
#include <unordered_map>

using namespace std;

typedef pair<long long, int> PLI;

void fibDijkstra(const WDigraph& graph, int startVertex, int endVertex, unordered_map<int, PLI> &tree);

void dijkstra(const WDigraph& graph, int startVertex, 
  unordered_map<int, PLI>& tree);

#endif
