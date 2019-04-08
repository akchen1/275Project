#include "server_util.h"

void runServer() {
/* this function runs the server by first reading from a txt file which 
has the noise values. Then it determines the nodes and edges from the noise values read
and finally communicates with the arduino */

  // unordered_map to store node information. pointData stores xy coords, first in pair
  // stores the vertexID while second in pair stores cost of tile
  unordered_map<pointData, pair<int,int>, pointHash> vertex;
  vector<double> noise; // noise values from txtfile
  WDigraph graph; // graph to store edges

  readFile(noise, "mapdata.txt"); // get noisevalues from txtfile
  generateMap(vertex, noise, graph);  // convert noise values to x,y and vertexID
  findEdge(graph, vertex);  // find edges connecting each vertex

  while (true) {
    string line = waitRequest();  // wait for a request from arudino
    pointData start, end;
    processRequest(line, start, end); // process the request
    int startnode = closest_point(vertex, start); // find closest startnode
    int endnode = closest_point(vertex, end); // find closest endnode
    list<pointData> path = getPath(vertex, graph, startnode, endnode, start, end); // compute paths
    communicate(path);  // send path to arduino
  }
}

int main() {
  runServer();
  return 0;
}