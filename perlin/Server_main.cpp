#include "Perlin.h"
#include "perlin.cpp"
#include <vector>
#include <iostream>
#include <string>
#include "serialport.h"
#include "serialport.cpp"

using namespace std;

SerialPort Serial("/dev/ttyACM0");

void generateNoise(vector<double> &coords, const vector<int> perm) {
    int width = 240;
    int height = 240;
    for (int i = 0; i < height/10; i++) {
        for (int j = 0; j < width/10; j++) {
            double x = (double) j/ ((double) width);
            double y = (double) i/ ((double) width);

            double n = noise(10 * x, 10 * y, 0.8, perm);
            coords.push_back(n);
        }
    }
}

bool wait_confirmation(float waittime) {
/* This function waits response from the arudino or checks for timeout*/

  string line;
  line = Serial.readline(waittime); // read from serial monitor with timeout
  cout << line << endl;
  if (line == "" || line.substr(0,1) != "A") {  // if timeout or incorrect response set false
    return false;
  }
  return true;
}

void sendNoise(const vector<double> coords) {
    string line;
    do {
        line = Serial.readline();
        cout << "recieved " << line << endl;
    } while (line.substr(0,1) != "S");
    cout << "out" << endl;
    Serial.writeline("A\n");
    for (auto i : coords) {
        // Serial.writeline(to_string(i));
        Serial.writeline("hello");
        Serial.writeline("\n");
        // cout << "waiting" << endl;
        // cout << i << endl;
        bool confirm = wait_confirmation(1000);
        // if (!confirm) {
        //     return;
        // }
    }
    Serial.writeline("E\n");
}
int main() {
    vector<double> coords;
    vector<int> perm;   // mermutation vector
    fill(perm);
    generateNoise(coords, perm);
    sendNoise(coords);
    cout << "done" << endl;

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