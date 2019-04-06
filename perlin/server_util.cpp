

#include "server_util.h"

using namespace std;

// struct pointData {
//   int x;
//   int y;
//   bool operator==(const pointData &other) const {
//     return ((x==other.x && y==other.y));
//   }
// };

// struct pointHash {
//   size_t operator()(const pointData &data) const {
//     size_t res = 17;
//     res = res*31 + hash<int>()(data.x);
//     res = res*31 + hash<int>()(data.y);
//     return res;
//   }
// };

int manhattan(const pointData& pt1, const pointData& pt2) {
/* Returns the manhattan distances */

  return abs(pt1.x - pt2.x) + abs(pt1.y - pt2. y);
}

void readFile(vector<double>& noise, string fname) {
/* this function read noise values stored in a txt file 
and stores it in a vector */

  ifstream mapdata;
  mapdata.open(fname);
  string line;
  while (getline(mapdata, line)) {
    noise.push_back(stod(line));
  }
  mapdata.close();
}

void generateMap(unordered_map<pointData, pair<int, int>, pointHash> &vertex, const vector<double> noise,WDigraph &graph) {
/* this function takes the noise values and adds nodes (defined as less than 
a certain noise value) to a unordered_map that stores x,y position, node number,
and the cost. x,y coords and noise values are assigned depending on which submap
we are on
submap numbers
 | 1 | 2 | 3 | 4 | 5 |
 | 6 | 7 | 8 | 9 | 10|
 | 11| 12| 13| 14| 15|
 | 16| 17| 18| 19| 20|
 | 21| 22| 23| 24| 25|
*/

  pointData node;
  // used to store into struct pointnode
  int x = 0;
  int y = 0;
  int key = 0;
  // used to keep track of which map we are on
  int W_offset = 0;
  int H_offset = 0;
  int counter = 0;
  int mapnum = 1;

  for (auto i : noise) {  // iterate through all the noise values
    if (i < 0.1) {  // noise value less than 0.1 counted as ground
      node.x = x;
      node.y = y;
      graph.addVertex(key);
      vertex.insert({node, make_pair(key, 1)}); // 1 is the cost of ground
      key++;
    }
    else if (i < 0.2) { // noise value less than 0.2 counted as grass
      node.x = x;
      node.y = y;
      graph.addVertex(key);
      vertex.insert({node, make_pair(key, 2)}); // 2 is the cost of grass
      key++;
    }
    else if (i < 0.3) { // noise value less than 0.3 counted as water
      node.x = x;
      node.y = y;
      graph.addVertex(key);
      vertex.insert({node, make_pair(key, 3)}); // 3 is cost of water
      key++;
    }

    // used to adjust xy coords depending which mapnum we are on
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

string waitRequest() {
/* this function waits for arduino to send a request to the server */

  cout << "wating..." << endl;
  string line;
  do {
    line = Serial.readline();
  } while (line.substr(0,1) != "R");
  return line;
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

void communicate(list<pointData> path) {
/* this function communicates with the arduino. It first sends the
path size, then it sends the x,y coordinates asking for acknowledgement
where needed */

  bool confirm;
  // send path size
  Serial.writeline("S ");
  Serial.writeline(to_string(path.size()));
  Serial.writeline("\n");
  // check for confimation
  confirm = wait_confirmation(1000);
  if (!confirm) {
    cout << "error" << endl;
    return;
  }
  // send x,y coordinates
  for (auto i : path) {
    // send x and check for acknowledgement
    Serial.writeline(to_string(i.x));
    Serial.writeline("\n");
    confirm = wait_confirmation(1000);
    if (!confirm) {
      cout << "error" << endl;
      return;
    }
    // send y and check for acknowledgement
    Serial.writeline(to_string(i.y));
    Serial.writeline("\n");
    confirm = wait_confirmation(1000);
    if (!confirm) {
      cout << "error" << endl;
      return;
    }
  }
  Serial.writeline("E\n");  // send end
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
  return closestnode; // return closest
}

void processRequest(string line, pointData &start, pointData &end) {
/* this function processes input recived from arduino. it reads the string
and finds the spaces to differentiate between values. */

  line = line.substr(2);
  int loc;  // space locator
  loc = line.find(" "); // find space
  start.x = stoi(line.substr(0, loc));  // read until space and store as x
  line = line.substr(loc+1);  //  set line to next coordnate
  loc = line.find(" "); // find space
  start.y = stoi(line.substr(0, loc));  // read until space and store as y
  line = line.substr(loc+1);  // point to next pair of coordinates
  loc = line.find(" "); // find space
  end.x = stoi(line.substr(0, loc));  // read until space and store as x
  line = line.substr(loc+1);  //  set line to next coordnate
  loc = line.find(" "); // find space
  end.y = stoi(line.substr(0, loc));  // read until space and store as y
}

void findEdge(WDigraph &graph,const unordered_map<pointData, pair<int,int>, pointHash> vertex ) {
/* this function iterates through all the valid nodes and checkes the tiles around it and see
if a node exists there and if so, it will create an edge and compute the appropriate cost from
going to that node which is the sum of their individual tile costs. eg. if I am on a 
ground tile (cost 1) and I found a water (cost 3) tile to the right, then the cost for me to 
go from ground to water is (1 + 3 = 4) */

  pointData p;
  unordered_map<pointData, pair<int,int>, pointHash>::const_iterator find;
  for (auto i : vertex) {

    // check tile to the left
    p.x = i.first.x - 5;
    p.y = i.first.y;
    find = vertex.find(p);
    if (find != vertex.end()) { // if found point to the left
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }

    // check tile to the right
    p.x = i.first.x + 5;
    p.y = i.first.y;
    find = vertex.find(p);
    if (find != vertex.end()) { // if found point to the right
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }

    // check tile to the bottom
    p.x = i.first.x;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) { // if tile found underneath
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }

    // check tile to the top
    p.x = i.first.x;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) { // if tile found to the top
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }

    // check tile to the top-left
    p.x = i.first.x - 5;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) { // if tile found to the top-left
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }

    // check tile to the top-right
    p.x = i.first.x + 5;
    p.y = i.first.y - 5;
    find = vertex.find(p);
    if (find != vertex.end()) { // if tile found to the top right
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }

    // check tile to the bottom left
    p.x = i.first.x - 5;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) { // if tile is found to the bottom-left
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }

    // check tile to the bottom-right
    p.x = i.first.x + 5;
    p.y = i.first.y + 5;
    find = vertex.find(p);
    if (find != vertex.end()) { // if tile is found to the bottom right
      graph.addEdge(i.second.first, vertex.at(p).first, i.second.second+vertex.at(p).second);
    }
  }
}

list<pointData> getPath(const unordered_map<pointData, pair<int,int>, pointHash> vertex, const WDigraph graph, const int start, const int end,
  const pointData startCoords, const pointData endCoords) {
/* this function uses three different search methods to obtain a path. First is using
normal dijkastra with a binary heap. Second is using dijkstra using a fibonacci heap.
Third is using astar using a fibonacci heap. The time it takes for each to find the 
path is measured. */

  
  unordered_map<int, PLI> binarySearchTree; // search tree with bheap and dijkstra
  unordered_map<int, PLI> searchTree; // search tree with fibheap and dijkstra
  unordered_map<int, PLI> astarSearchTree; // search tree with fibheap and astar

  clock_t start_time;
  double length;
  list<int> vertexes;

  for(auto i : vertex) {  // add all vertex ids to a vector
    vertexes.push_front(i.second.first);
  }
  
  // running binary heap with dijkstra
  start_time = clock();
  bdijkstra(graph, start, binarySearchTree);
  length = (clock() - start_time) / (double) CLOCKS_PER_SEC;
  cout << "The Binary Heap Dijkstra's Running Time : " << length << endl;
 
  // running fibonnaci hepa with dijkstra
  start_time = clock();
  fibdijkstra(graph, start, searchTree, vertexes);
  length = (clock() - start_time) / (double) CLOCKS_PER_SEC;
  cout << "The Fibonnaci Heap Dijkstra's Running Time : " << length << endl;

  // running fibonnaci with astar
  start_time = clock();
  fibstar(start, end, vertex, graph, astarSearchTree, startCoords, endCoords);
  length = (clock() - start_time) / (double) CLOCKS_PER_SEC;
  cout << "The FibStar Running Time : " << length << endl;

  list<int> path;
  list<pointData> path1;

  if (searchTree.find(end) == searchTree.end()) {  // if end node not found
    return path1;
  }
  else {  // find the path
    int stepping = end;
    while (stepping != start) {
      path.push_front(stepping);

    // crawl up the search tree one step
      stepping = searchTree[stepping].second;
    }
    path.push_front(start);
  }

  // convert vertexID's in path to pointdata structs which store x,y coords
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
