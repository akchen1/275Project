#include "perlin.h"
#include <iostream>
#define MAP_WIDTH 1200
#define MAP_HEIGHT 1200

void createImage(const vector<double> coords) {
  ofstream mapdata;
  mapdata.open("mapdata.txt");
  mapdata.precision(5);
  mapdata.setf(ios::fixed, ios::floatfield);
  for (auto i : coords) {
    mapdata << i << endl;
  }
  mapdata.close();
}

void generateNoise(const vector<int> perm, vector<double>& coords) {
  int W_offset = 0;
  int H_offset = 0;
  for (int grid = 1; grid <= 25; grid ++) {
    for (int i = 0 + W_offset; i < MAP_WIDTH/5 + W_offset; i+=5) {  // y
      for (int j = 0 + H_offset; j < MAP_HEIGHT/5 + H_offset; j+=5) { // x
        double y = (double) i/ ((double) MAP_WIDTH);
        double x = (double) j/ ((double) MAP_HEIGHT);

        double n = noise( 20*x,  20*y, 0.8, perm);

        // double n = 20 * noise(x, y, 0.8, perm);
        // n = n - floor(n);
        n = abs(n);
        cout << j << " " << i << endl;
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

int main() {

  vector<int> perm;
  fill(perm);
  vector<double> coords;
  generateNoise(perm, coords);
  createImage(coords);

  return 0;
}
