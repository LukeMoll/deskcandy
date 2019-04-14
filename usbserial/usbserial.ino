#include <Adafruit_NeoPixel.h>

void setup() {
  init_LEDs();
  Serial.begin(9600);  
   pinMode(LED_BUILTIN, OUTPUT);
}


enum LED_Formats {
  RGB
};

enum Modes {
  solid
};

struct SolidData {
  uint8_t r,g,b,w;
};

String serialInput = "";
uint8_t num_leds = 1;
enum LED_Formats ledformat = RGB;
enum Modes mode = solid;
struct SolidData solid_data = {0,0,0,0};

Adafruit_NeoPixel LEDs;

void loop() {
  readSerial();
  if(mode == solid) {
    draw_solid();
  }
  LEDs.show();
  delay(10);
}

void readSerial() {
  int data = 0;
  while (Serial.available() > 0) {
    digitalWrite(LED_BUILTIN, LOW);
    data = Serial.read();
    serialInput += (char) data;
    if ('\n' == data || '\r' == data) {
      Serial.println(handleSerial(String(serialInput)));
      serialInput = "";
    }
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void init_LEDs() {
  LEDs = Adafruit_NeoPixel(num_leds, D1, NEO_GRB + NEO_KHZ800);
  LEDs.begin();
  LEDs.setBrightness(255);
}

String handleSerial(String i) {
  i.trim();
  switch(i.charAt(0)) {
    case '?': // poke
      return String("!");
    case 'v': // version
      return String("1");
    case 'q': // query
      // uhhh we'll come back to that
      return String("e2");
    case 'n': // number
      if(i.charAt(1) == ' ') {
        long n = i.substring(2).toInt();
        if(n > 0 && n < (1 << 8)) {
          num_leds = n;
          init_LEDs();
          return String("ok");
        } else {return String("e1");}
      } else {return String("e1");}
    case 'f': // format
      if(i.charAt(1) == ' ') {
        String fmt = i.substring(2);
        if(fmt == "rgb") {
          ledformat = RGB;
          init_LEDs();
          return String("ok");
        }
        else {return String("e1");}
      }
      else {return String("e1");}
    case 'm': // mode
      // We'll come back to this too
      return String("e2");
  } // end of single-letter instructions
  // I have a horrible feeling parsing like this is a Bad Idea
  // mainly because we cannot have any word instructions that begin with [?vqnf]
  
  if(i.startsWith("solid")) {
    if(i.length() >= 14) {
      solid_data.r = strtoul(i.substring(6,8).c_str(), NULL, 16) & 255;
      solid_data.g = strtoul(i.substring(9,11).c_str(), NULL, 16) & 255;
      solid_data.b = strtoul(i.substring(12,14).c_str(), NULL, 16) & 255;
      if(i.length() >= 17) {
        solid_data.w = strtoul(i.substring(15,17).c_str(), NULL, 16) & 255;
      }
      return String("ok");
    }
    else {return String("e1");}
  }
  
  
  return String("e0");
}

void draw_solid() { // should probably decide on a consistent case huh
  // also working with RGBW is gonna be... fun
  for(uint8_t i = 0; i < num_leds; i++) {
    LEDs.setPixelColor(i, LEDs.Color(solid_data.r, solid_data.g, solid_data.b));
  }
}
