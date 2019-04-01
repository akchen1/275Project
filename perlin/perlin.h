#ifndef _PERLIN_H_
#define _PERLIN_H_

#include <random>
#include <vector>
#include <utility>
#include <iostream>
#include <cmath>
#include <time.h>
using namespace std;

void fill(vector<int> &v);

double interpolate(double x, double a, double b);
double smooth(double x);
double Gradient(int hash, double x, double y, double z);
double noise(double x, double y, double z, const vector<int> coords);

#endif