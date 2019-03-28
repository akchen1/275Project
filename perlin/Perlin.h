#ifndef _PERLIN_H_
#define _PERLIN_H_

void fill(vector<int> &v);

double interpolate(double a, double b, double x);
double smooth(double x);
double Gradient(int hash, double x, double y, double z);
double noise(double x, double y, double z, vector<int> coords);






#endif