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
#define YP A2
#define XM A3
#define YM  5
#define XP  4
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

uint8_t height[2304]; // store height values
uint16_t pathx[500];  // store x values of path
uint16_t pathy[500];  // store y values of path
uint16_t pathsize = 0;  // store path size
// starting location for player
int playerX = 120;
int playerY = 120;
int oldX = playerX;
int oldY = playerY;

bool solve = false; // no request sent, dont draw path

uint8_t map_number = 1; // keep track of which map number we are on
/* | 1 | 2 | 3 | 4 | 5 |
   | 6 | 7 | 8 | 9 | 10|
   | 11| 12| 13| 14| 15|
   | 16| 17| 18| 19| 20|
   | 21| 22| 23| 24| 25|*/
uint16_t map_bounds[6] = {0, 240, 480, 720, 960, 1200}; // bounds of each map

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

bool timeout(unsigned long start, unsigned long duration) {
/* this function checks or arduino timeout */

  unsigned long currenttime = millis();
  if(currenttime > start + duration) {
    return true;
  }
  else {return false;}
}

void drawButtons() {
/* this function draws the two buttons onto the arduino */

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
/* this function converts the maps the mapnumber
to a yscale that can be used to find the correct y-coordinate */

  uint8_t newlevel = 4;
  if (number < 6) {newlevel = 0;}
  else if (number < 11) {newlevel = 1;}
  else if (number < 16) {newlevel = 2;}
  else if (number < 21) {newlevel = 3;}
  return newlevel;
}

void sendRequest(uint16_t x, uint16_t y, uint8_t map) {
/* this function sends a request to the server */

  // map y level
  uint8_t y_level = convert_y_mapnumber(map_number);
  uint8_t y_end_level = convert_y_mapnumber(map);

  // send request
  Serial.print("R ");
  Serial.print(playerX + ((map_number-1)%5)*240);
  Serial.print(" ");
  Serial.print(playerY + y_level*240);
  Serial.print(" ");
  Serial.print(x + ((map-1)%5)*240);
  Serial.print(" ");
  Serial.println(y + y_end_level*240);
  delay(50);

  solve = true; // set solve to be true
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
      return 1; // touched top button
    }
    else {
      return 2; // touched bottom button
    }
  }
}

void readtxt() {
/* this function reads noise values from a text file which correspond to the 
current map the player is on. The noise values will be converted into heights
and then drawn */

  int noise_counter = 0;

  // Depending which mapnumber we are on, it will read the corresponding location
  double pos; // position in text file
  switch(map_number) {
    case 1  : pos = 0;      break;
    case 2  : pos = 18432;  break;
    case 3  : pos = 36864;  break;
    case 4  : pos = 55296;  break;
    case 5  : pos = 73728;  break;
    case 6  : pos = 92160;  break;
    case 7  : pos = 110592; break;
    case 8  : pos = 129024; break;
    case 9  : pos = 147456; break;
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
  
  if(!file.seek(pos)) { // got to specified position in file
    Serial.println("seek failed");
  }
  
  // read from text file and take noise values and convert to height values
  while (file.available() && noise_counter < 2304) {
    char in_char = file.read();
    if (in_char == '\n' || in_char == '\r') {

      double noi = atof(buffer);
      calculateHeight(noise_counter, noi);  // calculate corresponding height
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
  generateMap();  // generate the map
}

void calculateHeight(const int i, const double val) {
/* this function takes the noise values and converts them to a height
value which corresponds to different tiles */

  if (val < 0.1) {height[i] = 0; }  // path
  else if (val < 0.2) {height[i] = 1;} // path outline
  else if (val < 0.3) {height[i] = 2;} // light grass
  else if (val < 0.6) {height[i] = 3;} // light water
  else if (val < 0.8) {height[i] = 4;} // dark grass
  else if (val < 0.9) {height[i] = 5;} // wall
  else if (val <= 1.0) {height[i] = 6;} // dark water
}

void colorMap(const uint8_t val, const uint16_t x, const uint16_t y, uint16_t size) {
/* this function takes the height number and draws an tile depending on the height number*/
  
  if (val == 0) {tft.fillRect(x, y, size, size, 0xD408); }  // path
  else if (val == 1) {tft.fillRect(x, y, size, size, 0xAAE4);} // path outline
  else if (val == 2) {tft.fillRect(x, y, size, size, 0x066C);} // light grass
  else if (val == 3) {tft.fillRect(x, y, size, size, 0x319F);} // light water
  else if (val == 4) {tft.fillRect(x, y, size, size, 0x4D2D);} // dark grass
  else if (val == 5) {tft.fillRect(x, y, size, size, 0x7BCF);} // wall
  else if (val == 6) {tft.fillRect(x, y, size, size, 0x001F);} // dark water
}

void generateMap() {
/* this function sends x,y coordinates of each map to function
colorMap() to draw the right tiles */

  uint16_t y = 0;
  uint16_t x = 0;

  for (int i = 0; i < 2304; i++) {
    uint8_t val = height[i];
    colorMap(val, x, y, 5);
    
    // used to change x,y when map edge reached
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
/* this function draws the path */

  for (int i = 0; i < pathsize - 1; i++) {
    // check if in map bounds
    if (pathx[i] > map_bounds[(map_number-1)%5] && pathx[i] < map_bounds[(map_number-1)%5+1]) {
      if (pathy[i] > map_bounds[convert_y_mapnumber(map_number)] && pathy[i] < map_bounds[convert_y_mapnumber(map_number)+1]) {

        // convert global x,y to map x,y
        uint16_t x = pathx[i]-((map_number-1)%5)*240;
        uint16_t y = pathy[i]-(convert_y_mapnumber(map_number)*240);
        uint16_t x1 = pathx[i+1]-((map_number-1)%5)*240;
        uint16_t y1 = pathy[i+1]-(convert_y_mapnumber(map_number)*240);

        // draw line with some thickness
        tft.drawLine(x,y,x1,y1,ILI9341_BLACK);
        tft.drawLine(x,y+1,x1,y1+1,ILI9341_BLACK);
        tft.drawLine(x,y-1,x1-1,y1,ILI9341_BLACK);
        tft.drawLine(x+1,y,x1+1,y1,ILI9341_BLACK);
        tft.drawLine(x-1,y,x1-1,y1,ILI9341_BLACK);
      }
    }
  }
}

void end(uint16_t &x, uint16_t &y, uint8_t &map) {
/* this function generates random end point */

  randomSeed(analogRead(4));
  x = random(0,240);
  y = random(0,240);
  map = random(1, 26);
}

bool checkHeight() {
/* this function checks the height of the tile the player is 
trying to move onto, if it is less than a certain value, then the
player can move there */

  uint16_t x_center = floor(constrain(playerX / 5, 0, 47));
  uint16_t y_center = floor(constrain(playerY / 5, 0, 47));
  uint8_t val = height[x_center+48*y_center];
  if (val > 3) {
    return false;
  }
  return true;
}

void movePlayer() {
/* this function moves the player by redrawing the player, but
it also checks which tiles the player is on and redraws those tiles too */

  uint16_t y_up = floor(constrain((oldY-2)/5, 0, 47));
  uint16_t y_down = floor(constrain((oldY+2)/5, 0, 47));
  uint16_t x_left = floor(constrain((oldX-2)/5, 0, 47));
  uint16_t x_right = floor(constrain((oldX+2)/5, 0, 47));

  uint16_t x_center = floor(constrain(oldX / 5, 0, 47));
  uint16_t y_center = floor(constrain(oldY / 5, 0, 47));
 

  if (x_right != x_left && y_up != y_down) {  // if player is on 4 tiles
    uint8_t val_TR = height[x_right+48*y_up]; // topright height value
    uint8_t val_TL = height[x_left+48*y_up];  // top left height value
    uint8_t val_BR = height[x_right+48*y_down]; // bottom right height value
    uint8_t val_BL = height[x_left+48*y_down];  // bottom left height value

    // redraw map
    colorMap(val_TR, 5*x_right,5*y_up, 5);
    colorMap(val_TL, 5*x_left,5*y_up, 5);
    colorMap(val_BR, 5*x_right,5*y_down, 5);
    colorMap(val_BL, 5*x_left,5*y_down, 5);
  }
  else if (x_right != x_left) { // player is on two different horizontal tiles
    uint8_t val_L = height[x_left+48*y_center]; // left height value
    uint8_t val_R = height[x_right+48*y_center];  // right height value

    // redraw map
    colorMap(val_L, 5*x_left,5*y_center, 5);
    colorMap(val_R, 5*x_right,5*y_center, 5);
  }
  else if (y_up != y_down) {  // player is on two different vertical tiles
    uint8_t val_U = height[x_center+48*y_up]; // top height value
    uint8_t val_D = height[x_center+48*y_down]; // bottom height value

    // redraw map
    colorMap(val_U, 5*x_center,5*y_up, 5);
    colorMap(val_D, 5*x_center,5*y_down, 5);
  }
  else {  // player is on single tile
    uint8_t val = height[x_center+48*y_center]; // current tile height
    colorMap(val, 5*x_center,5*y_center, 5);  // redraw map
  }

  tft.fillCircle(playerX, playerY, 2, ILI9341_RED); // redraw player
}

bool checkEnd(uint16_t x, uint16_t y) {
/* this function checks if the player is on the goal */

  if (abs(playerX-x) < 5) {
    if (abs(playerY-y) < 5) {
      return true;
    }
  }
  return false;
}

void displayEnd() {
/* this function displays a end screen */

  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(10, 100);
  tft.setTextSize(2);
  tft.println("You found the treasure!");
  tft.println();
  tft.println("Tap the screen to play again");

  // check if user touch to play again
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

  // regenerate map, buttons, player
  generateMap();
  drawButtons();
  movePlayer();
  solve = false;
}

void player() {
/* this function reads joystick and moves player. It checks if the a new map
needs to be drawn and checks if player reached end goal and checks if a button
has been pressed */

  uint16_t x;
  uint16_t y;
  uint8_t map;
  end(x,y,map); // generate end goal
  while (true) {
    int v = analogRead(JOY_VERT_ANALOG);
    int h = analogRead(JOY_HORIZ_ANALOG);
    bool playerMove = false;

    // joystick movement
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

    // edge of map, if need to go to new map
    if (playerX > MAP_WIDTH - 4 && map_number%5!= 0) {
      // shift right
      map_number++;
      readtxt();  // read map data from txt
      playerX = 5;
    }
    else if (playerX < 4 && map_number%5!= 1) {
      // shift left
      map_number--;
      readtxt();  // read map data from txt
      playerX = MAP_WIDTH - 5;
    }
    else if (playerY > MAP_HEIGHT - 4 && map_number < 21) {
      // shift down
      map_number+=5;
      readtxt();  // read map data from txt
      playerY = 5;
    }
    else if (playerY < 4 && map_number > 5) {
      // shift up
      map_number-=5;
      readtxt();  // read map data from txt
      playerY = MAP_HEIGHT - 5;
    }

    // check if player moved
    if (playerMove) {
      // check if player is allowed to move there
      if (!checkHeight()) {
        playerX = oldX;
        playerY = oldY;
      }
      else {  // move player
        movePlayer();
        delay(100);
        oldX = playerX;
        oldY = playerY;
      }
    }

    // check if player is on same mapnumber as goal map number
    if (map_number == map) {  // if it is draw the map
      tft.fillRect(x, y, 4, 4, ILI9341_RED);
      if(checkEnd(x,y)) { // check if player touched goal
        displayEnd(); // diplay end message
        end(x,y,map); // generate new end goal
      }
    }

    // if solve condition draw the path 
    if (solve) {
      drawPath();
    }

    // check for button press
    int touch = check_touch();
    if (touch == 1) { // generate new end goal
      end(x, y, map);
      delay(100);
    }
    else if (touch == 2) {  // solve 
      // send location
      sendRequest(x,y, map);
      read();
      drawPath();
    }
  }
}

void processBuffer(const char* buffer,bool &xy, int &counter) {
/* this function processes the buffer to see what kind of data has been transfered */

  if (buffer[0] == 'S') { // if S is read, compute number read
    pathsize = 0;
    int32_t factor = 1;
    for (int i = buf_len-1; i > 1;i--) {  // convert a char read in buffer to number
      pathsize += (buffer[i] - '0') * factor;
      factor *= 10;
    }
    
    if (pathsize == 0 || pathsize > 500) {  // check length of path
      pathsize = 0;
      return;
    }
    Serial.println("A");  // if passed check above, send acknowledgement
  }

  else if ((int) buffer[0] >= 48 && (int) buffer[0] <=57 ){  // check for valid input using ascii compare

    if (xy) { // x coordinate
      pathx[counter] = atoi(buffer);
      xy = false; // set next to y coord
    }
    else {  // y coordinate
      pathy[counter] = atoi(buffer);
      counter++;
      xy = true;  // set next to x coord
    }

    Serial.flush();
    Serial.println("A");  // send acknowledgement to server
  }
}

bool read() {
/* This function reads from serial monitor and stores the characters into
a buffer. if \n or \r is read, it will process the buffer. It will return true
if timesout, otherwise false. This code is based of simple_client.cpp */

  char in_char;
  int counter = 0;
  bool xy = true; // x is true
  Serial.flush();
  do {  // read in character
    if (Serial.available()) {
      starttime = millis();

      // read the incoming byte:
      in_char = Serial.read();  // read character

      // if end of line is received, waiting for line is done:
      if (in_char == '\n' || in_char == '\r') {
      // now we process the buffer
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
    }
  } while (in_char != 'E'); // read until exit given or error
  Serial.flush();
  return 0;                     // successfully read
}

int main() {
  setup();

  Serial.flush();
  readtxt();
  tft.fillCircle(playerX, playerY, 2, ILI9341_RED);
  player();
  Serial.end();
  return 0;
}