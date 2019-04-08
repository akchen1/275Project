/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  275 Project: perlin.cpp

  Program Description:
  This function uses the functions in perlin.h to generate a txt file
  with noise values which correspond to a tile in a map
*/
#include "perlin.h"
#include <string>

#define MAP_WIDTH 1200
#define MAP_HEIGHT 1200

void createImage(const vector<double> coords, string fname) {
/* this function writes the calculated noise values onto a text file */

  ofstream mapdata;
  mapdata.open(fname);
  mapdata.precision(5);
  mapdata.setf(ios::fixed, ios::floatfield);
  for (auto i : coords) {
    mapdata << i << endl;
  }
  mapdata.close();
}

void generateNoise(const vector<int> perm, vector<double>& coords) {
/* this function sets the noise values to the appropriate map number
with corresponding x,y coordinates */

  // used to keep track which submap we are on
  int W_offset = 0;
  int H_offset = 0;
  // iterate through each submap
  for (int grid = 1; grid <= 25; grid ++) {
    for (int i = 0 + W_offset; i < MAP_WIDTH/5 + W_offset; i+=5) {  // y
      for (int j = 0 + H_offset; j < MAP_HEIGHT/5 + H_offset; j+=5) { // x
        double y = (double) i/ ((double) MAP_WIDTH);
        double x = (double) j/ ((double) MAP_HEIGHT);

        double n = noise( 20*x,  20*y, 0.8, perm);
        n = abs(n);
        coords.push_back(n);
      }
    }
    // used to adjust x,y depending on which submap we are on
    H_offset += MAP_WIDTH/5;
    if (grid%5==0) {
      W_offset += MAP_HEIGHT/5;
      H_offset = 0;
    }
  }
}

int main() {

  vector<int> perm;
  fill(perm); // fill the gradient vector
  vector<double> coords;
  generateNoise(perm, coords);
  createImage(coords, "mapdata.txt");

  return 0;
}
