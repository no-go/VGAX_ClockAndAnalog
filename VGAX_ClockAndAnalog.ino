#include <VGAX.h>
#include <math.h>
#include <VGAXUtils.h>
 
#include "glyph.h"

#define BLACK  0
#define RED    1
#define GREEN  2
#define YELLOW 3

#define STOREAGE 60

#define WHEEL_ONE_PIN  3  //analog
#define WHEEL_TWO_PIN  1  //analog
#define HOUR_PIN       A4 //digital 
#define MINU_PIN       A2 //digital 

VGAX vga;
VGAXUtils vgaU;

const char str0[] PROGMEM="0";
const char str1[] PROGMEM="1";
const char str2[] PROGMEM="2";
const char str3[] PROGMEM="3";
const char str4[] PROGMEM="4";
const char str5[] PROGMEM="5";
const char str6[] PROGMEM="6";
const char str7[] PROGMEM="7";
const char str8[] PROGMEM="8";
const char str9[] PROGMEM="9";
const char strD[] PROGMEM=":";

byte hours    = 0;
byte minutes  = 0;
byte seconds  = 0;
byte tenth    = 0;

int wheelOnePosition;
int wheelTwoPosition;

struct Midways {
  byte _val[STOREAGE];
  int  _nxt;
  byte _max;
  byte _min;

  Midways() {
    _nxt = 0;
    for (int i=0; i<STOREAGE; ++i) { 
      _val[i] = 128;
    }
  }

  void add(byte val) {
    _val[_nxt] = val;
    _nxt++;
    if (_nxt == STOREAGE) {
      _nxt = 0;
    }
  }

  byte last() {
    int l = _nxt -1;
    if (l < 0) l += STOREAGE;
    return _val[l];
  }

  byte midget() {
    int mid = 0;
    _min = 255;
    _max = 0;
    for (int i=0; i<STOREAGE; ++i) {
      if (_val[i] > _max) _max=_val[i];
      if (_val[i] < _min) _min=_val[i];
      mid += _val[i];
    }
    
    return (mid/STOREAGE);
  }

  void draw(int x, float fak, byte col) {
    int id = _nxt-1;
    byte mid = midget();
    int y = VGAX_HEIGHT/2;
    
    byte lastx,lasty;
    byte dx = x + STOREAGE;
    short dy = y - fak*(_val[id] - mid);
    
    if (id < 0) id += STOREAGE;
    for (int i=0; i<STOREAGE; ++i) {
      lastx = dx;
      lasty = dy;
      
      dx = x+STOREAGE-i;
      dy = y - fak*(_val[id] - mid);
      if (dy < 0) dy = 0;
      if (dy > (VGAX_HEIGHT-2)) dy = VGAX_HEIGHT-2;
      vgaU.draw_line(lastx, lasty, dx, dy, col); 
      id--;
      if (id < 0) id += STOREAGE;
    }
  }
} daten1, daten2;

void vgaPrint(const char* str, byte x, byte y, byte color){
   vga.printPROGMEM((byte*)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT, FNT_NANOFONT_HEIGHT, 3, 1, str, x, y, color);
}

void tick() {
  tenth++;

  if (tenth > 9) {
    tenth=0;
    seconds++;
  }
  
  if (seconds >= 60) {
    minutes++; seconds = seconds%60;
  }
  if (minutes == 60) {
    hours++;
    minutes = 0;
  }
  if (hours >= 24) {
    hours = hours%24;
  }
}

void myFont(int x, int y, int val) {
  byte col = YELLOW;
  switch (val) {
    case 0:
      vgaPrint(str0, x, y, col);
      break;
    case 1:
      vgaPrint(str1, x, y, col);
      break;
    case 2:
      vgaPrint(str2, x, y, col);
      break;
    case 3:
      vgaPrint(str3, x, y, col);
      break;
    case 4:
      vgaPrint(str4, x, y, col);
      break;
    case 5:
      vgaPrint(str5, x, y, col);
      break;
    case 6:
      vgaPrint(str6, x, y, col);
      break;
    case 7:
      vgaPrint(str7, x, y, col);
      break;
    case 8:
      vgaPrint(str8, x, y, col);
      break;
    case 9:
      vgaPrint(str9, x, y, col);
      break;
    default:
      ;
  }
}

void digiClock() {
  int t = hours/10;
  myFont(28, 0, t);

  t = hours - t*10;
  myFont(36, 0, t);

  vgaPrint(strD, 45, 0, YELLOW);
  
  t = minutes/10;
  myFont(52, 0, t);
  
  t = minutes - t*10;
  myFont(60, 0, t);

  vgaPrint(strD, 69, 0, YELLOW);

  t = seconds/10;
  myFont(76, 0, t);
  
  t = seconds - t*10;
  myFont(84, 0, t);
}

void setup() {
  vga.begin();
  vga.clear(BLACK);
}

void loop() {
  wheelOnePosition = analogRead(WHEEL_ONE_PIN);
  wheelTwoPosition = analogRead(WHEEL_TWO_PIN);
  vga.clear(BLACK);
  daten1.add(wheelOnePosition/8);
  daten2.add(wheelTwoPosition/8);
  digiClock();
  daten1.draw( 0, 1.0, RED);
  daten2.draw(59, 1.0, GREEN);
  
  if (digitalRead(HOUR_PIN) == HIGH) {
    hours = (hours+1)%24;
    tenth=0; seconds=0;
    vga.delay(300);
  }
  if (digitalRead(MINU_PIN) == HIGH) {
    minutes = (minutes+1)%60;
    tenth=0; seconds=0;
    vga.delay(300);
  }
  
  vga.delay(80); // should be 100 -> but I have to much code
  tick();
}

