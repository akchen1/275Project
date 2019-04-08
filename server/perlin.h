/*
  Name: MingYang Mao (1544517)
        Andrew Chen (1531348)
  CCID: mmao, akchen
  Course: CMPUT 275 Winter 2019
  275 Project: perlin.h

  Program Description:
  This program is based on perlin noise by Ken Perlin.
  https://mrl.nyu.edu/~perlin/noise/
*/
#ifndef _PERLIN_H_
#define _PERLIN_H_

#include <random>
#include <vector>
#include <utility>
#include <cmath>
#include <time.h>
#include <fstream>
using namespace std;

void fill(vector<int> &v) {
/* Generate a vector of random numbers */
  srand(time(NULL));
  for (int i = 0; i < 256; i++) {
      v.push_back(rand()%255);

  } 
  v.insert(v.end(), v.begin(), v.end());
}

/* IMPORTANT!
    These functions below are based and taken from Ken Perlin who
    invented Perlin noise
    https://mrl.nyu.edu/~perlin/noise/
*/
double interpolate(double x, double a, double b) {
  return a + x *(b-a);
}

double smooth(double x) {
  return 6*pow(x,5) - 15*pow(x,4) + 10*pow(x,3);
}

double Gradient(int hash, double x, double y, double z) {

  int h = hash & 15;

  double u = h < 8 ? x : y,
       v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);

}

double noise(double x, double y, double z, const vector<int> coords) {

  int x0 = (int) floor(x) & 255; // & 255 to insure we dont index out
  int y0 = (int) floor(y) & 255;
  int z0 = (int) floor(z) & 255;
  
  double sx = x - (double) floor(x);
  double sy = y - (double) floor(y);
  double sz = z - (double) floor(z);

  double u = smooth(sx);
  double v = smooth(sy);
  double w = smooth(sz);

  // hash coordinates
  int A = coords[x0] +y0;
  int AA = coords[A] + z0;
  int AB = coords[A+1] + z0;
  int B = coords[x0 + 1] + y0;
  int BA = coords[B] + z0;
  int BB = coords[B+1] + z0;


  double GradAA = Gradient(coords[AA], sx, sy, sz);
  double GradBA = Gradient(coords[BA], sx-1, sy, sz);
  double GradAB = Gradient(coords[AB], sx, sy - 1, sz);
  double GradBB = Gradient(coords[BB], sx-1, sy-1, sz);

  double GradAA1 = Gradient(coords[AA+1], sx, sy, sz-1);
  double GradBA1 = Gradient(coords[BA+1], sx-1, sy, sz-1);
  double GradAB1 = Gradient(coords[AB+1], sx, sy - 1, sz-1);
  double GradBB1 = Gradient(coords[BB+1], sx-1, sy-1, sz-1);

  double Top_x = interpolate(u, GradAA, GradBA);
  double Top_y = interpolate(u, GradAB, GradBB);
  double Bot_x = interpolate(u, GradAA1, GradBA1);
  double Bot_y = interpolate(u, GradAB1, GradBB1);

  double value = interpolate(w, interpolate(v,Top_x,Top_y), interpolate(v, Bot_x, Bot_y));
  return value;

}

#endif