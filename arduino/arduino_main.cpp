#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include <TouchScreen.h>


#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6
#define JOY_VERT_ANALOG A1
#define JOY_HORIZ_ANALOG A0
#define JOY_DEADZONE 64
#define JOY_CENTRE 512
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
#define MINPRESSURE   10
#define MAXPRESSURE 1000
// touch screen pins, obtained from the documentaion
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM  5  // can be a digital pin
#define XP  4  // can be a digital pin
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
File file;

#define MAP_WIDTH  240
#define MAP_HEIGHT 240
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

const uint16_t buf_size = 256;  // buffer size
uint16_t buf_len = 0; // buffer length
char* buffer = (char *) malloc(buf_size); // allocate memory
// uint16_t noise[1600];
// uint16_t noise[2304];
uint8_t height[2304] = {0};
uint16_t pathx[500];
uint16_t pathy[500];
uint16_t pathsize = 0;
int playerX = 120;
int playerY = 120;
int oldX = playerX;
int oldY = playerY;
unsigned long bookmark = 0;
bool solve = false;

uint8_t map_number = 1;
/* | 1 | 2 | 3 | 4 | 5 |
   | 6 | 7 | 8 | 9 | 10|
   | 11| 12| 13| 14| 15|
   | 16| 17| 18| 19| 20|
   | 21| 22| 23| 24| 25|*/
uint16_t map_bounds[6] = {0, 240, 480, 720, 960, 1200};

void generateMap();
void calculateHeight(const int i, const double val);
void drawButtons();
bool read();

void setup() {
  init();
  Serial.begin(9600);
  Serial.flush();

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  // Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed! Is it inserted properly?");
    while (true) {}
  }

  file = SD.open("mapdata.txt", FILE_READ);
  if (!file) {
    Serial.println("failed to open");
  }

  buf_len = 0;  // set values
  buffer[buf_len] = 0;  // set values
  drawButtons();
}

// bool timeout(unsigned long start, unsigned long duration) {
// /* this function checks or arduino timeout */

//   unsigned long currenttime = millis();
//   if(currenttime > start + duration) {
//     return true;
//   }
//   else {return false;}
// }

void drawButtons() {
  // draw top right box for rating selector
  tft.fillRect(MAP_WIDTH, 1, DISPLAY_WIDTH - MAP_WIDTH, (DISPLAY_HEIGHT-1)/2, ILI9341_WHITE);
  tft.drawRect(MAP_WIDTH, 1, DISPLAY_WIDTH - MAP_WIDTH, (DISPLAY_HEIGHT-1)/2, ILI9341_RED);
  
  tft.setTextSize(3);
  tft.setTextColor(0x0000, 0xFFFF);
  tft.setCursor(290, 45);

  // draw sorting cycle option (lower right)
  tft.fillRect(MAP_WIDTH, (DISPLAY_HEIGHT-1)/2 + 4, DISPLAY_WIDTH - MAP_WIDTH, (DISPLAY_HEIGHT-1)/2 - 4, ILI9341_WHITE);
  tft.drawRect(MAP_WIDTH, (DISPLAY_HEIGHT-1)/2 + 2, DISPLAY_WIDTH - MAP_WIDTH, (DISPLAY_HEIGHT-1)/2, ILI9341_GREEN);
  

  // tft.setCursor(290, 160);
  tft.setTextSize(2);


  tft.drawChar(MAP_WIDTH+35, 40, 'N', ILI9341_BLACK, ILI9341_WHITE, 2);
  tft.drawChar(MAP_WIDTH+35, 60, 'E', ILI9341_BLACK, ILI9341_WHITE, 2);
  tft.drawChar(MAP_WIDTH+35, 80, 'W', ILI9341_BLACK, ILI9341_WHITE, 2);

  tft.drawChar(MAP_WIDTH+35, 132, 'S', ILI9341_BLACK, ILI9341_WHITE, 2);
  tft.drawChar(MAP_WIDTH+35, 152, 'O', ILI9341_BLACK, ILI9341_WHITE, 2);
  tft.drawChar(MAP_WIDTH+35, 172, 'L', ILI9341_BLACK, ILI9341_WHITE, 2);
  tft.drawChar(MAP_WIDTH+35, 192, 'V', ILI9341_BLACK, ILI9341_WHITE, 2);
  tft.drawChar(MAP_WIDTH+35, 212, 'E', ILI9341_BLACK, ILI9341_WHITE, 2);
}

uint8_t convert_y_mapnumber(uint8_t number) {
  uint8_t newlevel = 4;
  if (number < 6) {newlevel = 0;}
  else if (number < 11) {newlevel = 1;}
  else if (number < 16) {newlevel = 2;}
  else if (number < 21) {newlevel = 3;}
  return newlevel;
}

void sendRequest(uint16_t x, uint16_t y, uint8_t map) {
  // uint16_t x_start = floor(constrain(playerX / 5, 0, 47));
  // uint16_t y_start = floor(constrain(playerY / 5, 0, 47));
  uint8_t y_level = convert_y_mapnumber(map_number);
  uint8_t y_end_level = convert_y_mapnumber(map);

  Serial.print("R ");
  Serial.print(playerX + ((map_number-1)%5)*240);
  Serial.print(" ");
  Serial.print(playerY + y_level*240);
  Serial.print(" ");
  Serial.print(x + ((map-1)%5)*240);
  Serial.print(" ");
  Serial.println(y + y_end_level*240);
  delay(50);
  solve = true;
}

int check_touch() {
/* check where the map is touched*/

    TSPoint p = ts.getPoint();
    if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
        return 0;
    }
    int screen_x = map(p.y, TS_MINY, TS_MAXY, DISPLAY_WIDTH, 0);
    int screen_y = map(p.x, TS_MINY, TS_MAXY, DISPLAY_HEIGHT, 0);
    if (screen_x >= MAP_WIDTH) {   // if touch in map
      if (screen_y >= (DISPLAY_HEIGHT-1)/2) {
        return 1;
      }
      else {
        return 2;                       // condition for map touch
      }
    }
}

void readtxt() {
  // uint16_t noise[2304];
  
  int noise_counter = 0;
  // Serial.println(map_number);
  double pos;
  switch(map_number) { // this is ugly :(
    case 1 : pos = 0; break;
    case 2 : pos = 18432; break;
    case 3 : pos = 36864; break;
    case 4 : pos = 55296; break;
    case 5 : pos = 73728; break;
    case 6 : pos = 92160; break;
    case 7 : pos = 110592; break;
    case 8 : pos = 129024; break;
    case 9 : pos = 147456; break;
    case 10 : pos = 165888; break;
    case 11 : pos = 184320; break;
    case 12 : pos = 202752; break;
    case 13 : pos = 221184; break;
    case 14 : pos = 239616; break;
    case 15 : pos = 258048; break;
    case 16 : pos = 276480; break;
    case 17 : pos = 294912; break;
    case 18 : pos = 313344; break;
    case 19 : pos = 331776; break;
    case 20 : pos = 350208; break;
    case 21 : pos = 368640; break;
    case 22 : pos = 387072; break;
    case 23 : pos = 405504; break;
    case 24 : pos = 423936; break;
    case 25 : pos = 442368; break;
    default : Serial.println("error");
  }
  
  // double pos = 18432*(map_number-1);
  // Serial.println(pos);
  if(!file.seek(pos)) { // 18432
    Serial.println("bookmark failed");
  }
  // Serial.println("reading");
  while (file.available() && noise_counter < 2304) {
    char in_char = file.read();
    // Serial.println(in_char);
    if (in_char == '\n' || in_char == '\r') {
      // Serial.println(buffer);

      double noi = atof(buffer);
      calculateHeight(noise_counter, noi);
      // Serial.println(noi);
      noise_counter++;

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
  generateMap();
  // Serial.println("done reading");
  // bookmark = file.position();
  // Serial.println(bookmark);
}

void calculateHeight(const int i, const double val) {
  if (val < 0.1) {height[i] = 0; }  // path??
  else if (val < 0.2) {height[i] = 1;} // path outline??
  else if (val < 0.3) {height[i] = 2;} // light grass
  else if (val < 0.6) {height[i] = 3;} // light water
  else if (val < 0.8) {height[i] = 4;} // dark grass
  else if (val < 0.9) {height[i] = 5;} // wall
  else if (val <= 1.0) {height[i] = 6;} // dark water
}

void colorMap(const uint8_t val, const uint16_t x, const uint16_t y, uint16_t size) {
  
  if (val == 0) {tft.fillRect(x, y, size, size, 0xD408); }  // path??
  else if (val == 1) {tft.fillRect(x, y, size, size, 0xAAE4);} // path outline??
  else if (val == 2) {tft.fillRect(x, y, size, size, 0x066C);} // light grass
  else if (val == 3) {tft.fillRect(x, y, size, size, 0x319F);} // light water
  else if (val == 4) {tft.fillRect(x, y, size, size, 0x4D2D);} // dark grass
  else if (val == 5) {tft.fillRect(x, y, size, size, 0x7BCF);} // wall
  else if (val == 6) {tft.fillRect(x, y, size, size, 0x001F);} // dark water
}

void generateMap() {
  // Serial.println("Generating map");
  uint16_t y = 0;
  uint16_t x = 0;

  for (int i = 0; i < 2304; i++) {
    uint8_t val = height[i];
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


void drawPath() {
    for (int i = 0; i < pathsize - 1; i++) {
      // Serial.print(pathx[i]);
      // Serial.print(" ");
      // Serial.println(pathy[i]);
        if (pathx[i] > map_bounds[(map_number-1)%5] && pathx[i] < map_bounds[(map_number-1)%5+1]) {
            if (pathy[i] > map_bounds[convert_y_mapnumber(map_number)] && pathy[i] < map_bounds[convert_y_mapnumber(map_number)+1]) {
                uint16_t x = pathx[i]-((map_number-1)%5)*240;
                uint16_t y = pathy[i]-(convert_y_mapnumber(map_number)*240);
                uint16_t x1 = pathx[i+1]-((map_number-1)%5)*240;
                uint16_t y1 = pathy[i+1]-(convert_y_mapnumber(map_number)*240);
                tft.drawLine(x,y,x1,y1,ILI9341_BLACK);
            }
        }
    }
}

void end(uint16_t &x, uint16_t &y, uint8_t &map) {
  randomSeed(analogRead(4));
  x = random(0,240);
  y = random(0,240);
  map = random(1, 26);
  // x = 200;
  // y = 200;
  // map = 1;
  // x = floor(constrain(x / 5, 0, 47));
  // y = floor(constrain(y / 5, 0, 47));
}

bool checkHeight() {  // need fix
  uint16_t x_center = floor(constrain(playerX / 5, 0, 47));
  uint16_t y_center = floor(constrain(playerY / 5, 0, 47));
  uint8_t val = height[x_center+48*y_center];
  // Serial.println(val);
  if (val > 3) {
    return false;
  }
  return true;
}

void movePlayer() {
  uint16_t y_up = floor(constrain((oldY-2)/5, 0, 47));
  uint16_t y_down = floor(constrain((oldY+2)/5, 0, 47));
  uint16_t x_left = floor(constrain((oldX-2)/5, 0, 47));
  uint16_t x_right = floor(constrain((oldX+2)/5, 0, 47));

  uint16_t x_center = floor(constrain(oldX / 5, 0, 47));
  uint16_t y_center = floor(constrain(oldY / 5, 0, 47));
 

  if (x_right != x_left && y_up != y_down) {  // if player is on 4 tiles
    // Serial.println("player is on 4");
    uint8_t val_TR = height[x_right+48*y_up];
    uint8_t val_TL = height[x_left+48*y_up];
    uint8_t val_BR = height[x_right+48*y_down];
    uint8_t val_BL = height[x_left+48*y_down];

    colorMap(val_TR, 5*x_right,5*y_up, 5 );
    colorMap(val_TL, 5*x_left,5*y_up, 5 );
    colorMap(val_BR, 5*x_right,5*y_down, 5 );
    colorMap(val_BL, 5*x_left,5*y_down, 5 );
  }
  else if (x_right != x_left) { // player is on two different horizontal tiles
    // Serial.println("player is on 2 horizontal");
    uint8_t val_L = height[x_left+48*y_center];
    uint8_t val_R = height[x_right+48*y_center];

    colorMap(val_L, 5*x_left,5*y_center, 5 );
    colorMap(val_R, 5*x_right,5*y_center, 5 );
  }
  else if (y_up != y_down) {  // player is on two different vertical tiles
    // Serial.println("player is on 2 vertical");
    uint8_t val_U = height[x_center+48*y_up];
    uint8_t val_D = height[x_center+48*y_down];

    colorMap(val_U, 5*x_center,5*y_up, 5 );
    colorMap(val_D, 5*x_center,5*y_down, 5 );
  }
  else {  // player is on single tile
    // Serial.println("player is on 1");
    uint8_t val = height[x_center+48*y_center];
    colorMap(val, 5*x_center,5*y_center, 5 );
  }
  tft.fillCircle(playerX, playerY, 2, ILI9341_RED);
}

bool checkEnd(uint16_t x, uint16_t y) {

  if (abs(playerX-x) < 2) {
    if (abs(playerY-y) < 2) {
      return true;
    }
  }
  return false;
}

void displayEnd() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(0, 120);
  tft.setTextSize(3);
  tft.println("You found the treasure!");
  tft.println();
  tft.println("Tap the screen to play again");
  bool touch = false;
  while (!touch) {
    TSPoint p = ts.getPoint();
    if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
      touch = false;
    }
    else {
      touch = true;
    }
  }
  generateMap();
  drawButtons();
}

void player() {
  uint16_t x;
  uint16_t y;
  uint8_t map;
  end(x,y,map);
  while (true) {
    int v = analogRead(JOY_VERT_ANALOG);
    int h = analogRead(JOY_HORIZ_ANALOG);
    bool playerMove = false;

    if (abs(v - JOY_CENTRE) > JOY_DEADZONE) {
      int delta = (v - JOY_CENTRE) / 64;
      playerY = constrain(playerY + delta, 0, MAP_HEIGHT);
      playerMove |= (playerY != oldY);
    }

    if (abs(h - JOY_CENTRE) > JOY_DEADZONE) {
      int delta = -(h - JOY_CENTRE) / 64;
      playerX = constrain(playerX + delta, 0, MAP_HEIGHT-3);
      playerMove |= (playerX != oldX);
    }

    if (playerX > MAP_WIDTH - 4 && map_number%5!= 0) {
      // shift right
      map_number++;
      readtxt();
      playerX = 5;
    }
    else if (playerX < 4 && map_number%5!= 1) {
      // shift left
      map_number--;
      readtxt();
      playerX = MAP_WIDTH - 5;
    }
    else if (playerY > MAP_HEIGHT - 4 && map_number < 21) {
      // shift down
      map_number+=5;
      readtxt();
      playerY = 5;
    }
    else if (playerY < 4 && map_number > 5) {
      // shift up
      map_number-=5;
      readtxt();
      playerY = MAP_HEIGHT - 5;
    }

    if (playerMove) {
      // Serial.println("checking height");
      if (!checkHeight()) {
        playerX = oldX;
        playerY = oldY;
      }
      else {
        // Serial.println("movingplayer");
        movePlayer();
        // drawPath();
        delay(100);
        oldX = playerX;
        oldY = playerY;
      }
    }

    if (map_number == map) {
      tft.fillRect(x, y, 4, 4, ILI9341_RED);
      if(checkEnd(x,y)) {
        displayEnd();
        end(x,y,map);
      }
    }

    if (solve) {
      drawPath();
    }
    if (check_touch() == 1) {
      end(x, y, map);
      delay(100);
    }
    else if (check_touch() == 2) {
      // send location
      sendRequest(x,y, map);
      read();
      drawPath();
    }

  }
}

void processBuffer(const char* buffer,bool &xy, int &counter) {
  if (buffer[0] == 'S') { // if N is read, compute number read
    pathsize = 0;
    int32_t factor = 1;
    for (int i = buf_len-1; i > 1;i--) {  // convert a char read in buffer to number
      pathsize += (buffer[i] - '0') * factor;
      factor *= 10;
    }
     
    if (pathsize == 0 || pathsize > 500) {  // check length of waypoints
      pathsize = 0;
      return;
    }
    Serial.println("A");  // if passed check above, send acknowledgement
  }

  else {  // if W is read, compute waypoint coords

    if (xy) {
      pathx[counter] = atoi(buffer);
      xy = false;
    }
    else {
      pathy[counter] = atoi(buffer);
      counter++;
      xy = true;
    }

    Serial.flush();
    Serial.println("A");  // send achknowledgement to server
    // Serial.print("A ");
    // Serial.println(buffer);
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
  bool xy = true; // x is true

  do {  // read in character
    // bool timed = timeout(starttime, duration);
    // Serial.print("wait");
    if (Serial.available()) { // if timeout
      // starttime = millis();
      // duration = 1000;                  // reset start time and change waiting time to 1s
      // read the incoming byte:
      in_char = Serial.read();  // read character

      // if end of line is received, waiting for line is done:
      if (in_char == '\n' || in_char == '\r') {
      // now we process the buffer
        
        // Serial.println(buffer);
        processBuffer(buffer, xy, counter);
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

  } while (in_char != 'E'); // read until exit given or error
  return 0;                     // successfully read
}

// bool recieveNoise() {
//     // Serial.flush();
//     // char in = 'Z';
//     Serial.println("S");
//     // do {
//     //   Serial.println("S");
//     //   if (Serial.available()) {
//     //     in = Serial.read();
//     //   }
//     // } while (in != 'A');
//     // Serial.println("S");
//     Serial.flush();
    
    
//     read();
//     return true;
// }

int main() {
  setup();

    
  Serial.flush();
  // recieveNoise();
  readtxt();
  // generateMap();
  tft.fillCircle(playerX, playerY, 2, ILI9341_RED);
  player();
  
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