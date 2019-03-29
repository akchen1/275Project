/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  Assignment 2: server_util.h
*/

#ifndef __SERVER_UTIL_H
#define __SERVER_UTIL_H

#include "digraph.h"
#include "wdigraph.h"
#include "heap.h"
#include "dijkstra.h"
#include "serialport.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <list>
#include <queue>
#include <cmath>
#include <cassert>

using namespace std;

SerialPort Serial("/dev/ttyACM0");

struct Point {
    long long lat;  // latitude of the point
    long long lon;  // longitude of the point
};

long long manhattan(const Point& pt1, const Point& pt2) {
/* Returns the manhattan distances */

  return abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2. lon);
}

void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points) {
/* Reads from textfile to create a weighted graph and stores the vertices into an 
unordered map.*/
  
  ifstream file;
  int ID, start, end, commaLoc;         // vertex ID, start and end edges, commaLoc for comma location in file lines
  string name, line, letter;            // string for name, the current line from file and letter that is read in

  float lat, lon;

  Point coords;

  file.open(filename.c_str());
  while(getline(file, line)) {
    // first find the letter to handle task type
    // while loop finds the commas in each line and obtains the data given in such format
    commaLoc = line.find(",");
    letter = line.substr(0, commaLoc);
    line = line.substr(commaLoc+1);

    // letter V is identified
    if(letter.compare("V") == 0) {
      // get vertex ID 
      commaLoc = line.find(",");
      ID = stoi(line.substr(0, commaLoc));            // obtain the vertex ID (convert string->int)
      line = line.substr(commaLoc+1);

      // // get vertex lon, lat (no implementation for this weekly)
      commaLoc = line.find(",");
      lat = stold(line.substr(0, commaLoc));  // convert to long double
      lon = stold(line.substr(commaLoc+1)); // convert to long double

      coords.lat = (long long) (lat * 100000);
      coords.lon = (long long) (lon * 100000);

      points.insert({ID, coords});  // insert into unordered_map
    }

    // letter E case
    else if (letter.compare("E") == 0) {
      
      commaLoc = line.find(",");
      start = stoi(line.substr(0, commaLoc));      // obtain start (string -> int)
      line = line.substr(commaLoc+1);              // substring the string to one char past comma

      commaLoc = line.find(",");
      end = stoi(line.substr(0, commaLoc));
      // name = stoi(line.substr(commaLoc+1));
      
      long long cost = manhattan(points.at(start), points.at(end)); // calculate cost
      graph.addEdge(start, end, cost);  // add edge to wgraph
    }
  }
}

int closest_point(const unordered_map<int, Point> points, const Point coord) {
/* This function finds the closest node from the coordinates inputed*/

  long long smallest; // smallest node
  long long compare;  // used to compare values
  int closestnode = -1;
  bool first = true;

  for (auto i : points) { // iterate through all the nodes to find the closest node
    compare = manhattan(i.second, coord);
    if (first) {
      smallest = compare; // set smallest node as first node
      first = false;
    }
    if (compare < smallest) { // if next node is smaller than current smallest
      smallest = compare; // set smallest to next
      closestnode = i.first;  // save vertexid of closest 
    }
  }
  return closestnode; // return vertexID
}

list<int> getPath(const unordered_map<int, Point> nodes, const WDigraph graph, const Point start, const Point end) {
/* This function calles dijkstra and calculates the shortest path then waits for
arduino response and outputs the number of waypoints and their coordnates when a 
resonse is recieved */

  unordered_map<int, PLI> searchTree;
  int start_vertex = closest_point(nodes, start); // start_vertex vertex
  int end_vertex = closest_point(nodes, end);  // end vertex
  dijkstra(graph, start_vertex, searchTree); // get path from dijkstra

  list<int> path;

  // This secton of the code is taken from dijkstra.cpp and modified for this assignment
  if (searchTree.find(end_vertex) == searchTree.end()) {  // if end node not found
    return path;
  }
  else {
    int stepping = end_vertex;
    while (stepping != start_vertex) {
      path.push_front(stepping);


    // crawl up the search tree one step
      stepping = searchTree[stepping].second;
    }
    path.push_front(start_vertex);

    return path;
  }
}

Point process_input(string& line) {
/* This function proccesses the waypoints recived and returns them as points*/

  Point coords;
  int loc;  // space locator
  loc = line.find(" "); // find space
  coords.lat = stoll(line.substr(0, loc));  // read until space and store as lat
  line = line.substr(loc+1);  //  set line to next coordnate
  loc = line.find(" "); // find space
  coords.lon = stoll(line.substr(0, loc));  // read until space and store as lon
  line = line.substr(loc+1);  // point to next pair of coordinates
  return coords;

}

bool wait_confirmation(float waittime) {
/* This function waits response from the arudino or checks for timeout*/

  string line;
  line = Serial.readline(waittime); // read from serial monitor with timeout
  
  if (line == "" || line.substr(0,1) != "A") {  // if timeout or incorrect response set false
    return false;
  }
  return true;
}

void communicate(const unordered_map<int, Point> nodes, const list<int> path) {
/* This function communicates with the ardunino, sending number of waypoints and 
the coordinates of the waypoints*/

  if (path.size() == 0) { // if no path send no path to arduino
    Serial.writeline("N 0\n");
    return;
  }
  else {  // if there is a path send path length
    Serial.writeline("N ");
    Serial.writeline(to_string(path.size()));
    Serial.writeline("\n");
    bool confirm = wait_confirmation(1000);  // check for arduino confirmation

    if (!confirm) { // if no response from arduino
      return;
    }

    for (auto it : path) {  // iterate through list of waypoints and send to arduino
      Serial.writeline("W ");
      Serial.writeline(to_string(nodes.at(it).lat));
      Serial.writeline(" ");
      Serial.writeline(to_string(nodes.at(it).lon));
      Serial.writeline("\n");
      bool confirm = wait_confirmation(1000); // check for arduino confirmation

      if (!confirm) {  // if no response from arduino
        return;
      }
    }

    Serial.writeline("E\n");  // send end
  }
}

void server(const unordered_map<int, Point> nodes, const WDigraph graph) {
/* This function waits for request from arduino, processes the request, 
computes the path and communicates with the arduino*/
  while (true) {
    string line;
    Point start, end;

    do {
      line = Serial.readline();
    } while (line.substr(0,1) != "R");  // wait for arduino request

    line = line.substr(2);  // request recived, set line to coords only

    start = process_input(line); // get start point
    end = process_input(line);  // get end point

    list<int> path;
    path = getPath(nodes, graph, start, end); // compute path
    communicate(nodes, path);   // communicate path with arduino
  }
}

#endif