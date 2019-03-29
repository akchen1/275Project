#include <Arduino.h>
#include <Adafruit_ILI9341.h>


#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

const uint16_t buf_size = 256;  // buffer size
uint16_t buf_len = 0; // buffer length
char* buffer = (char *) malloc(buf_size); // allocate memory
double noise[576];

void setup() {
    init();
    Serial.begin(9600);
    Serial.flush();

    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(ILI9341_BLACK);

    buf_len = 0;  // set values
    buffer[buf_len] = 0;  // set values
}

bool timeout(unsigned long start, unsigned long duration) {
/* this function checks or arduino timeout */

  unsigned long currenttime = millis();
  if(currenttime > start + duration) {
    return true;
  }
  else {return false;}
}

bool read() {
/* This function reads from serial monitor and stores the characters into
a buffer. if \n or \r is read, it will process the buffer. It will return true
if timesout, otherwise false. This code is based of simple_client.cpp */
  // Serial.println("ddsfo");
  
  char in_char;
  unsigned long starttime = millis();             // start timer
  unsigned long duration = 10000;                 // initially set to 10s
  int counter = 0;

  do {  // read in character
    bool timed = timeout(starttime, duration);
    // Serial.println("d121o");
    if (Serial.available() && !timed) { // if timeout
      starttime = millis();
      duration = 1000;                  // reset start time and change waiting time to 1s
      // read the incoming byte:
      in_char = Serial.read();  // read character

      // if end of line is received, waiting for line is done:
      if (in_char == '\n' || in_char == '\r') {
      // now we process the buffer
        // if (counter == 0) {
        //   tft.setCursor(0,10);
        //   tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        //   tft.setTextSize(2);
        //   tft.println(buffer);
        // }
        Serial.flush();
        Serial.println(buffer);
        noise[counter] = atof(buffer); // process the buffer
        counter++;
        Serial.flush();
        Serial.println("A");

        // clear the buffer
        
        buffer[buf_len] = 0;
        buf_len = 0;
      }

      else {
          // add character to buffer, provided that we don't overflow.
          // drop any excess characters.
        if ( buf_len < buf_size-1 ) {
          buffer[buf_len] = in_char;
          buf_len++;
          buffer[buf_len] = 0;
        }
      }
      // Serial.println("do");
    }

    else if (timed) {return 1;}          // timeout issue

  } while (in_char != 'E'); // read until exit given or error
  return 0;                     // successfully read
}

bool recieveNoise() {
    // Serial.flush();
    char in = 'Z';
    do {
      Serial.println("S");
      if (Serial.available()) {
        in = Serial.read();
      }
    } while (in != 'A');
    // Serial.println("S");
    // Serial.flush();
    
    
    read();
    return true;
}

int main() {
  setup();

    
  Serial.flush();
  recieveNoise();
  
  for (int i = 0; i<576;i++) {
    Serial.println(noise[i]);
    Serial.println("work?");
  }
  //Serial.println("S");
    Serial.end();
    return 0;
}