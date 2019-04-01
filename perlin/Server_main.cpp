#include "perlin.h"
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

using namespace std;

#define MAP_WIDTH 1200
#define MAP_HEIGHT 1200

SerialPort Serial("/dev/ttyACM0");

struct pointData {
  int x;
  int y;
  bool operator==(const pointData &other) const {
    return ((x==other.x && y==other.y));
  }
};

struct pointHash {
  size_t operator()(const pointData &data) const {
    size_t res = 17;
    res = res*31 + hash<int>()(data.x);
    res = res*31 + hash<int>()(data.y);
    return res;
  }
};

void createImage(const vector<double> coords) {
  ofstream mapdata;
  mapdata.open("mapdata.txt");
  mapdata.precision(5);
  mapdata.setf(ios::fixed, ios::floatfield);
  for (auto i : coords) {
    // cout << i.first << endl;
    mapdata << i << endl;
  }
  mapdata.close();
}

void generateNoise(unordered_map<pointData, pair<int, int>, pointHash> &vertex, const vector<int> perm, vector<double>& coords) {
  int W_offset = 0;
  int H_offset = 0;
  pointData node;
  int key = 0;
  for (int grid = 1; grid <= 25; grid ++) {
    for (int i = 0 + W_offset; i < MAP_WIDTH/5 + W_offset; i+=5) {  // x
      for (int j = 0 + H_offset; j < MAP_HEIGHT/5 + H_offset; j+=5) { // y
        double x = (double) i/ ((double) MAP_WIDTH);
        double y = (double) j/ ((double) MAP_HEIGHT);

        double n = noise( 0.5*x,  0.5*y, 0.8, perm);

        // double n = 20 * noise(x, y, 0.8, perm);
        // n = n - floor(n);
        n = abs(n);
        if (n < 0.1) {
          node.x = i;
          node.y = j;
          vertex.insert({node, make_pair(key, 0)});
          key++;
        }
        else if (n < 0.2) {
          node.x = i;
          node.y = j;
          vertex.insert({node, make_pair(key, 1)});
          key++;
        }
        else if (n < 0.3) {
          node.x = i;
          node.y = j;
          vertex.insert({node, make_pair(key, 2)});
          key++;
        }

        
        coords.push_back(n);
        // cout << n << " ";
        // cout << n/1000 << endl;
      }
    }
    H_offset += MAP_WIDTH/5;
    if (grid%5==0) {
      W_offset += MAP_HEIGHT/5;
      H_offset = 0;
    }
    
  }
}

bool wait_confirmation(float waittime) {
/* This function waits response from the arudino or checks for timeout*/

  string line;
  line = Serial.readline(waittime); // read from serial monitor with timeout
  cout << "got " << line;
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
        // return;
  }
  for (auto i : path) {
    cout << i.x << " " << i.y << endl;
    Serial.writeline(to_string(i.x));
    Serial.writeline("\n");
    confirm = wait_confirmation(1000);
    if (!confirm) {
        cout << "error" << endl;
        // return;
    }
    Serial.writeline(to_string(i.y));
    Serial.writeline("\n");
    confirm = wait_confirmation(1000);
    if (!confirm) {
        cout << "error" << endl;
        // return;
    }
  }
  cout <<"sending E" <<endl;
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
    cout << "recieved " << line << endl;
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
    }
    p.x = i.first.x + 5;
    p.y = i.first.y;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the right" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
    p.x = i.first.x;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the bottom" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
    p.x = i.first.x;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the top" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
    p.x = i.first.x - 5;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the top-left" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
    p.x = i.first.x + 5;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the top-right" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
    p.x = i.first.x - 5;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the bottom-left" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
    p.x = i.first.x + 5;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) {
      // cout << "point to the bottom-right" << endl;
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
  }
}

list<pointData> getPath(const unordered_map<pointData, pair<int,int>, pointHash> vertex, const WDigraph graph, const int start, const int end) {
  unordered_map<int, PLI> searchTree;
  // int start_vertex = closest_point(nodes, start); // start_vertex vertex
  // int end_vertex = closest_point(nodes, end);  // end vertex
  
  //dijkstra(graph, start, searchTree); // get path from dijkstra
  
  fibDijkstra(graph, start, end, searchTree, vertex);

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
  return path1;
}


int main() {
    vector<double> coords;
    unordered_map<pointData, pair<int,int>, pointHash> vertex;
    WDigraph graph;
    vector<int> perm;   // mermutation vector
    fill(perm);
    generateNoise(vertex, perm, coords);
    createImage(coords);
    findEdge(graph, vertex);
    string line = waitRequest();
    pointData start, end;
    processRequest(line, start, end);
    int startnode = closest_point(vertex, start);
    int endnode = closest_point(vertex, end);


    // cout << startnode << " " << endnode << endl;
    // cout << start.x << " " << start.y << " " << end.x << " "<<end.y <<endl;
    list<pointData> path = getPath(vertex, graph, startnode, endnode); 
    // cout << path.size() << endl;
    communicate(path);
    // waitRequest();
    // for (i : path) {
    //   cout << i << endl;
    // }
    cout << "done" << endl;

    
    return 0;
}