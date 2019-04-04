#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include "serialport.h"
#include "digraph.h"
#include "wdigraph.h"
#include "dijkstra.h"
#include <unordered_map>
#include <fstream>
#include <utility>
#include <random>
#include <cmath>
#include <list>

#include "astar.h"
#include "server_util.h"
#include <ctime>


using namespace std;

#define MAP_WIDTH 1200
#define MAP_HEIGHT 1200

SerialPort Serial("/dev/ttyACM0");



void readFile(vector<double>& noise) {
  ifstream mapdata;
  mapdata.open("mapdata.txt");
  string line;
  while (getline(mapdata, line)) {
    // cout << line << endl;

    noise.push_back(stod(line));
  }
  mapdata.close();
}

void generateNoise(unordered_map<pointData, pair<int, int>, pointHash> &vertex, const vector<double> noise,WDigraph &graph) {
  pointData node;
  int W_offset = 0;
  int H_offset = 0;
  int x = 0;
  int y = 0;
  int key = 0;
  int counter = 0;
  int mapnum = 1;
  for (auto i : noise) {
    if (i < 0.1) {  //ground
      node.x = x;
      node.y = y;
      graph.addVertex(key);
      vertex.insert({node, make_pair(key, 1)});
      key++;
    }
    else if (i < 0.2) { //green
      node.x = x;
      node.y = y;
      graph.addVertex(key);
      vertex.insert({node, make_pair(key, 2)});
      key++;
    }
    else if (i < 0.3) { //water
      node.x = x;
      node.y = y;
      graph.addVertex(key);
      vertex.insert({node, make_pair(key, 3)});
      key++;
    }
    // cout << x << " " << y << endl;
    x+=5;
    if ((x+W_offset)%240 == 0) {
      y += 5;
      x = W_offset;
      counter++;
    }
    if (counter == 48) {
      counter = 0;
      y = H_offset;
      W_offset += 240;
      x = W_offset;
      
      
      if (mapnum%5==0) {
        W_offset = 0;
        x = 0;
        H_offset += 240;
        y = H_offset;
      }
      mapnum++;
    }

  }
}

bool wait_confirmation(float waittime) {
/* This function waits response from the arudino or checks for timeout*/

  string line;
  line = Serial.readline(waittime); // read from serial monitor with timeout
  // cout << "got " << line;
  if (line == "" || line.substr(0,1) != "A") {  // if timeout or incorrect response set false
    return false;
  }
  return true;
}

void communicate(list<pointData> path) {
  bool confirm;
  // Serial.writeline("A\n");
  Serial.writeline("S ");
  Serial.writeline(to_string(path.size()));
  Serial.writeline("\n");
  confirm = wait_confirmation(1000);
  if (!confirm) {
    cout << "error" << endl;
    return;
  }
  for (auto i : path) {
    // cout << i.x << " " << i.y << endl;
    Serial.writeline(to_string(i.x));
    Serial.writeline("\n");
    confirm = wait_confirmation(1000);
    if (!confirm) {
      cout << "error" << endl;
      return;
    }
    Serial.writeline(to_string(i.y));
    Serial.writeline("\n");
    confirm = wait_confirmation(1000);
    if (!confirm) {
      cout << "error" << endl;
      return;
    }
  }
  // cout <<"sending E" <<endl;
  Serial.writeline("E\n");
}

int manhattan(const pointData& pt1, const pointData& pt2) {
/* Returns the manhattan distances */

  return abs(pt1.x - pt2.x) + abs(pt1.y - pt2. y);
}

int closest_point(const unordered_map<pointData, pair<int,int>, pointHash> vertex, const pointData &point) {
/* This function finds the closest node from the coordinates inputed*/

  int smallest; // smallest node
  int compare;  // used to compare values
  int closestnode = -1;
  bool first = true;

  for (auto i : vertex) { // iterate through all the nodes to find the closest node
    compare = manhattan(i.first, point);
    if (first) {
      smallest = compare; // set smallest node as first node
      first = false;
    }
    if (compare < smallest) { // if next node is smaller than current smallest
      smallest = compare; // set smallest to next
      closestnode = i.second.first;  // save vertexid of closest
    }
  }
  return closestnode; // return vertexID
}

void processRequest(string line, pointData &start, pointData &end) {
  line = line.substr(2);
  int loc;  // space locator
  loc = line.find(" "); // find space
  start.x = stoi(line.substr(0, loc));  // read until space and store as lat
  line = line.substr(loc+1);  //  set line to next coordnate
  loc = line.find(" "); // find space
  start.y = stoi(line.substr(0, loc));  // read until space and store as lon
  line = line.substr(loc+1);  // point to next pair of coordinates
  loc = line.find(" "); // find space
  end.x = stoi(line.substr(0, loc));  // read until space and store as lat
  line = line.substr(loc+1);  //  set line to next coordnate
  loc = line.find(" "); // find space
  end.y = stoi(line.substr(0, loc));  // read until space and store as lon
};

string waitRequest() {
  cout << "wating..." << endl;
  string line;
  do {
    line = Serial.readline();
    // cout << "recieved " << line << endl;
  } while (line.substr(0,1) != "R");
  // cout << "recieved" << endl;
  return line;
}

void findEdge(WDigraph &graph,const unordered_map<pointData, pair<int,int>, pointHash> vertex ) {
  pointData p;
  unordered_map<pointData, pair<int,int>, pointHash>::const_iterator find;
  for (auto i : vertex) {
    // cout << i.first << endl;
    p.x = i.first.x - 5;
    p.y = i.first.y;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the left" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }
    p.x = i.first.x + 5;
    p.y = i.first.y;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the right" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }
    p.x = i.first.x;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the bottom" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }
    p.x = i.first.x;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the top" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }
    p.x = i.first.x - 5;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the top-left" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }
    p.x = i.first.x + 5;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the top-right" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }
    p.x = i.first.x - 5;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the bottom-left" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }
    p.x = i.first.x + 5;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the bottom-right" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
      // cout << i.second.second+vertex.at(p).second << endl;
    }

  }
}

list<pointData> getPath(const unordered_map<pointData, pair<int,int>, pointHash> vertex, const WDigraph graph, const int start, const int end,
  const pointData startCoords, const pointData endCoords) {
  unordered_map<int, PLI> searchTree;
  unordered_map<int, PLI> binarySearchTree;
  unordered_map<int, PLI> astarSearchTree; 

  clock_t start_time;
  double length;
  // int start_vertex = closest_point(nodes, start); // start_vertex vertex
  // int end_vertex = closest_point(nodes, end);  // end vertex
  //dijkstra(graph, start, searchTree); // get path from dijkstra
  list<int> vertexes;


  for(auto i : vertex) {
    vertexes.push_front(i.second.first);
  }
  
  // dijkstra(graph, start, searchTree);
  // fibDijkstra(graph, start, end, searchTree, vertexes); // get path from dijkstra
  // astar(start, end, vertex,graph);

  start_time = clock();
  bdijkstra(graph, start, binarySearchTree);
  length = (clock() - start_time) / (double) CLOCKS_PER_SEC;
  cout << "The Binary Heap Dijkstra's Running Time : " << length << endl;
 
  
  start_time = clock();
  fibdijkstra(graph, start, searchTree, vertexes); // get path from dijkstra
  length = (clock() - start_time) / (double) CLOCKS_PER_SEC;
  cout << "The Fibonnaci Heap Dijkstra's Running Time : " << length << endl;

  start_time = clock();
  fibstar(start, end, vertex, graph, astarSearchTree, startCoords, endCoords); // get path from dijkstra
  length = (clock() - start_time) / (double) CLOCKS_PER_SEC;
  cout << "The FibStar Running Time : " << length << endl;

  list<int> path;
  list<pointData> path1;

  // This secton of the code is taken from dijkstra.cpp and modified for this assignment
  if (searchTree.find(end) == searchTree.end()) {  // if end node not found
    return path1;
  }
  else {
    int stepping = end;
    while (stepping != start) {
      path.push_front(stepping);


    // crawl up the search tree one step
      stepping = searchTree[stepping].second;
    }
    path.push_front(start);
  }
  for (auto i : path) {
    for (auto j : vertex) {
      if (j.second.first == i) {
        path1.push_back(j.first);
        break;
      }
    }
  }
  // cout << path1.size() << endl;
  return path1;
}


int main() {
    vector<double> noise;
    unordered_map<pointData, pair<int,int>, pointHash> vertex;
    WDigraph graph;
    readFile(noise);
    generateNoise(vertex, noise, graph);
    findEdge(graph, vertex);
    while (true) {
      string line = waitRequest();
      pointData start, end;
      processRequest(line, start, end);
      int startnode = closest_point(vertex, start);
      int endnode = closest_point(vertex, end);
      list<pointData> path = getPath(vertex, graph, startnode, endnode, start, end); 
      communicate(path);
    }
    


    
    return 0;
}