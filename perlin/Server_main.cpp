#include "Perlin.h"
#include "perlin.cpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include "serialport.h"
#include "serialport.cpp"
#include <fstream>
#include <cmath>

using namespace std;

#define MAP_WIDTH 1200
#define MAP_HEIGHT 1200

SerialPort Serial("/dev/ttyACM0");

void createImage(vector<double> &coords) {
  ofstream mapdata;
  mapdata.open("mapdata.txt");
  mapdata.precision(5);
  mapdata.setf(ios::fixed, ios::floatfield);
  for (auto i : coords) {
     // cout << to_string(i).size() << endl;
    // if (to_string(i).size() < 10) {
    //   mapdata << i;

    //   for (unsigned int j = 0; j < 10- to_string(i).size(); j++) {  // padding
    //     mapdata << "*";
    //   }
    //   mapdata << endl;
    // }
    // else {
    //   mapdata << i << endl;
    // }

    mapdata << i << endl;
  }
  mapdata.close();
}

void generateNoise(vector<double> &coords, const vector<int> perm) {
  int W_offset = 0;
  int H_offset = 0;
  for (int grid = 1; grid <= 25; grid ++) {
    for (int i = 0 + W_offset; i < MAP_WIDTH/25 + W_offset; i++) {
      for (int j = 0 + H_offset; j < MAP_HEIGHT/25 + H_offset; j++) {
        double x = (double) j/ ((double) MAP_WIDTH);
        double y = (double) i/ ((double) MAP_HEIGHT);

        double n = noise(20 * x, 20 * y, 0.8, perm);

        // double n = 20 * noise(x, y, 0.8, perm);
        // n = n - floor(n);
        n = abs(n);
        
       
        coords.push_back(n);
        // cout << n << " ";
        // cout << n/1000 << endl;
      }
    }
    if (grid%5==0) {
      H_offset += MAP_HEIGHT/5;
      W_offset = 0;
    }
    W_offset += MAP_WIDTH/5;
  }
}

bool wait_confirmation(float waittime) {
/* This function waits response from the arudino or checks for timeout*/

  string line;
  line = Serial.readline(waittime); // read from serial monitor with timeout
  // cout << "got " << line;
  if (line == "" || line.substr(0,1) != "A") {  // if timeout or incorrect response set false
    return false;
  }
  return true;
}

void sendNoise(const vector<double> coords) {
    string line;
    do {
        line = Serial.readline();
        // cout << "recieved " << line << endl;
    } while (line.substr(0,1) != "S");
    cout << "out" << endl;
    // Serial.writeline("A\n");
    for (auto i : coords) {
        string noiseval = to_string(i);
        size_t loc = noiseval.find(".");
        Serial.writeline(noiseval.substr(0, loc));
        // Serial.writeline("hello");
        Serial.writeline("\n");
        // cout << "expected " << i << endl;
        // cout << i << endl;
        bool confirm = wait_confirmation(1000);
        if (!confirm) {
            cout << "error" << endl;
            // return;
        }
    }
    cout <<"sending E" <<endl;
    Serial.writeline("E\n");

}
int main() {
    vector<double> coords;
    vector<int> perm;   // mermutation vector
    fill(perm);
    generateNoise(coords, perm);
    // sendNoise(coords);
    createImage(coords);
    cout << "done" << endl;

    // while (true) {
    //     wait_confirmation(1000);
    // }
    

    // string line;
    // do {
    //     line = Serial.readline();
    //     cout << line << endl;
    // } while (true);

    // for (auto i : coords){

    //     cout << i << endl;
    // }
    // cout << coords.size() << endl;


    return 0;
}