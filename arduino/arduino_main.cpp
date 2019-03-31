#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>


#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6
#define JOY_VERT_ANALOG A1
#define JOY_HORIZ_ANALOG A0
#define JOY_DEADZONE 64
#define JOY_CENTRE 512

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

const uint16_t buf_size = 256;  // buffer size
uint16_t buf_len = 0; // buffer length
char* buffer = (char *) malloc(buf_size); // allocate memory
// double noise[576];
uint16_t noise[2304];
int playerX = 10;
int playerY = 10;
int oldX = playerX;
int oldY = playerY;
unsigned long bookmark = 0;

uint8_t map_number = 1;
/* | 1 | 2 | 3 | 4 | 5 |
   | 6 | 7 | 8 | 9 | 10|
   | 11| 12| 13| 14| 15|
   | 16| 17| 18| 19| 20|
   | 21| 22| 23| 24| 25|*/


void setup() {
  init();
  Serial.begin(9600);
  Serial.flush();

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed! Is it inserted properly?");
    while (true) {}
  }

  buf_len = 0;  // set values
  buffer[buf_len] = 0;  // set values
}

// bool timeout(unsigned long start, unsigned long duration) {
// /* this function checks or arduino timeout */

//   unsigned long currenttime = millis();
//   if(currenttime > start + duration) {
//     return true;
//   }
//   else {return false;}
// }


void readtxt() {
  File file;
  file = SD.open("mapdata.txt", FILE_READ);
  if (!file) {
    Serial.println("failed to open");
  }
  int noise_counter = 0;
  file.seek(bookmark);
  while (file.available() && noise_counter < 2304) {
    char in_char = file.read();
    // Serial.println(in_char);
    if (in_char == '\n' || in_char == '\r') {
      // Serial.println(buffer);

    
      noise_counter++;
      double noise_value = atof(buffer)*1000;
      noise[noise_counter] = noise_value;
      

      buf_len = 0;
      buffer[buf_len] = 0;

    }
    else {
      if ( buf_len < buf_size-1 ) {
        buffer[buf_len] = in_char;
        buf_len++;
        buffer[buf_len] = 0;
      }
    }
  }
  bookmark = file.position();
  Serial.println(bookmark);
}

void colorMap(const double val, const uint16_t x, const uint16_t y, uint16_t size) {
  
  if (val < 0.1) {tft.fillRect(x, y, size, size, 0xD408);}  // path??
  else if (val < 0.2) {tft.fillRect(x, y, size, size, 0xAAE4);} // path outline??
  else if (val < 0.3) {tft.fillRect(x, y, size, size, 0x066C);} // light grass
  else if (val < 0.6) {tft.fillRect(x, y, size, size, 0x7BCF);} // wall
  else if (val < 0.8) {tft.fillRect(x, y, size, size, 0x4D2D);} // dark grass
  
  else if (val < 0.9) {tft.fillRect(x, y, size, size, 0x319F);} // light water
  else if (val <= 1.0) {tft.fillRect(x, y, size, size, 0x001F);} // dark water
}

void generateMap() {

  uint16_t y = 0;
  uint16_t x = 0;
  for (int i = 0; i < 2304; i++) {
    double val = ((double) abs(noise[i]))/1000;
    // Serial.println(val);
    colorMap(val, x, y, 5);
    
    // tft.fillRect(x, y, 10, 10, tft.color565(abs(noise[i])*0x10, abs(noise[i])*0x005f00, abs(noise[i])*0x0000d7));
    x += 5;
    if (x == 240) {
      x = 0;
      y += 5;
    }
    if (y == 240) {
      return;
    }
  }
}

bool checkHeight() {
  uint16_t x_center = floor(constrain(playerX / 5, 0, 47));
  uint16_t y_center = floor(constrain(playerY / 5, 0, 47));
  double val = ((double) abs(noise[x_center+48*y_center]))/1000;
  if (val > 0.2) {
    return false;
  }
}

void movePlayer() {
  uint16_t y_up = floor(constrain((oldY-2)/5, 0, 47));
  uint16_t y_down = floor(constrain((oldY+2)/5, 0, 47));
  uint16_t x_left = floor(constrain((oldX-2)/5, 0, 47));
  uint16_t x_right = floor(constrain((oldX+2)/5, 0, 47));

  uint16_t x_center = floor(constrain(oldX / 5, 0, 47));
  uint16_t y_center = floor(constrain(oldY / 5, 0, 47));

  // Serial.print(y_up);
  // Serial.print(" ");
  // Serial.print(y_down);
  // Serial.print(" ");
  // Serial.print(x_left);
  // Serial.print(" ");
  // Serial.print(x_right);
  // Serial.print(" ");
  // Serial.print(x_center);
  // Serial.print(" ");
  // Serial.println(y_center);
 

  if (x_right != x_left && y_up != y_down) {  // if player is on 4 tiles
    Serial.println("player is on 4");
    double val_TR = ((double) abs(noise[x_right+48*y_up]))/1000;
    double val_TL = ((double) abs(noise[x_left+48*y_up]))/1000;
    double val_BR = ((double) abs(noise[x_right+48*y_down]))/1000;
    double val_BL = ((double) abs(noise[x_left+48*y_down]))/1000;

    colorMap(val_TR, 5*x_right,5*y_up, 5 );
    colorMap(val_TL, 5*x_left,5*y_up, 5 );
    colorMap(val_BR, 5*x_right,5*y_down, 5 );
    colorMap(val_BL, 5*x_left,5*y_down, 5 );
  }
  else if (x_right != x_left) { // player is on two different horizontal tiles
    Serial.println("player is on 2 horizontal");
    double val_L = ((double) abs(noise[x_left+48*y_center]))/1000;
    double val_R = ((double) abs(noise[x_right+48*y_center]))/1000;

    colorMap(val_L, 5*x_left,5*y_center, 5 );
    colorMap(val_R, 5*x_right,5*y_center, 5 );
  }
  else if (y_up != y_down) {  // player is on two different vertical tiles
    Serial.println("player is on 2 vertical");
    double val_U = ((double) abs(noise[x_center+48*y_up]))/1000;
    double val_D = ((double) abs(noise[x_center+48*y_down]))/1000;

    colorMap(val_U, 5*x_center,5*y_up, 5 );
    colorMap(val_D, 5*x_center,5*y_down, 5 );
  }
  else {  // player is on single tile
    Serial.println("player is on 1");
    double val = ((double) abs(noise[x_center+48*y_center]))/1000;
    colorMap(val, 5*x_center,5*y_center, 5 );
  }




  

  tft.fillCircle(playerX, playerY, 2, ILI9341_RED);
  // Serial.print(playerX);
  // Serial.print(" ");
  // Serial.print(playerY);
  // Serial.print(" ");
  // Serial.print(5*x);
  // Serial.print(" ");
  // Serial.print(5*y);
  // Serial.print(" ");
  // Serial.println(x+48*y);
}

void player() {
  int v = analogRead(JOY_VERT_ANALOG);
  int h = analogRead(JOY_HORIZ_ANALOG);
  bool playerMove = false;

  if (abs(v - JOY_CENTRE) > JOY_DEADZONE) {
    int delta = (v - JOY_CENTRE) / 64;
    playerY = constrain(playerY + delta, 0, DISPLAY_HEIGHT);
    playerMove |= (playerY != oldY);
  }

  if (abs(h - JOY_CENTRE) > JOY_DEADZONE) {
    int delta = -(h - JOY_CENTRE) / 64;
    playerX = constrain(playerX + delta, 0, DISPLAY_HEIGHT-3);
    playerMove |= (playerX != oldX);
  }

  if (playerX > DISPLAY_WIDTH - 4 && map_number%5!= 0) {
    // shift right
    map_number++;
  }
  else if (playerX < 4 && map_number%5!= 1) {
    // shift left
    map_number--;
  }
  else if (playerY > DISPLAY_HEIGHT - 4 && map_number < 21) {
    // shift down
    map_number+=5;
  }
  else if (playerY < 4 && map_number > 5) {
    // shift up
    map_number-=5;
  }

  if (playerMove) {
    if (!checkHeight()) {
      playerX = oldX;
      playerY = oldY;
    }
    else {
      movePlayer();
      delay(100);
      oldX = playerX;
      oldY = playerY;
    }
    
    
  }
}

bool read() {
/* This function reads from serial monitor and stores the characters into
a buffer. if \n or \r is read, it will process the buffer. It will return true
if timesout, otherwise false. This code is based of simple_client.cpp */
  // Serial.println("ddsfo");
  
  char in_char;
  // unsigned long starttime = millis();             // start timer
  // unsigned long duration = 10000;                 // initially set to 10s
  int counter = 0;

  do {  // read in character
    // bool timed = timeout(starttime, duration);
    // Serial.println("d121o");
    if (Serial.available()) { // if timeout
      // starttime = millis();
      // duration = 1000;                  // reset start time and change waiting time to 1s
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
        Serial.println("A");
        // Serial.flush();
        // Serial.print(" ");
        
        
        noise[counter] = atof(buffer); // process the buffer
        // Serial.println(noise[counter]);
        counter++;
        // Serial.flush();
        // clear the buffer
        buf_len = 0;
        buffer[buf_len] = 0;

        
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

    // else if (timed) {return 1;}          // timeout issue

  } while (counter!=2304); // read until exit given or error
  return 0;                     // successfully read
}

bool recieveNoise() {
    // Serial.flush();
    // char in = 'Z';
    Serial.println("S");
    // do {
    //   Serial.println("S");
    //   if (Serial.available()) {
    //     in = Serial.read();
    //   }
    // } while (in != 'A');
    // Serial.println("S");
    Serial.flush();
    
    
    read();
    return true;
}

int main() {
  setup();

    
  Serial.flush();
  // recieveNoise();
  readtxt();
  generateMap();
  tft.fillCircle(playerX, playerY, 2, ILI9341_RED);
  
  while (true) {
    player();
  }
  // while (true) {
    
  // }
  
  // for (int i = 0; i<2304;i++) {
  //   Serial.println(noise[i]);
  //   // Serial.println("work?");
  // }
  //Serial.println("S");
    Serial.end();
    return 0;
}