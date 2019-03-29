#include <cmath>
#include "ppm.h"
#include "ppm.cpp"
#include "perlin.cpp"
#include <vector>
#include <iostream>
using namespace std;

int main() {
	// Define the size of the image
	unsigned int width = 600, height = 450;

	// Create an empty PPM image
	ppm image(width, height);

	// Create a PerlinNoise object with a random permutation vector generated with seed
	vector<int> p;
	fill(p);

	unsigned int kk = 0;
	// Visit every pixel of the image and assign a color generated with Perlin noise
	for(unsigned int i = 0; i < height; ++i) {     // y
		for(unsigned int j = 0; j < width; ++j) {  // x
			double x = (double)j/((double)width);
			double y = (double)i/((double)height);

			// Typical Perlin noise
			double n = noise(10 * x, 10 * y, 0.8,p);
			cout << n << endl;
			// Wood like structure
			// double n = 20 * noise(x, y, 0.8, p);
			// n = n - floor(n);
			// cout << "?" << endl;
			// Map the values to the [0, 255] interval, for simplicity we use 
			// tones of grey
			image.r[kk] = floor(255 * n);
			image.g[kk] = floor(255 * n);
			image.b[kk] = floor(255 * n);

			// image.r[kk] = 255 * n;
			// image.g[kk] = 255 * n;
			// image.b[kk] = 255 * n;
			kk++;
		}
	}

	// Save the image in a binary PPM file
	image.write("figure_7_P.ppm");

	return 0;
}