#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN D1
#define NUM_PIXELS 72

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
//  strip.setBrightness(255);
  strip.setBrightness(32);
  strip.show(); // Initialize all pixels to 'off'
}

uint8_t prog = 0;
uint8_t segWidths[] = {
  16,
  32,
  48,
  64,
  96,
  128,
  192,
  255
};
#define NUM_SEGWIDTHS 8
uint8_t segWidthIndex = NUM_SEGWIDTHS - 1;
uint8_t pressed = false;

void loop() {
  prog = rainbowSegment(10,segWidths[segWidthIndex], prog);
  if(digitalRead(D2) == LOW) { // switch is active-low
    pressed = true;
  }
  else if(pressed) { // D2 is high again and we've been waiting for it to be released
    pressed = false;
    segWidthIndex = (segWidthIndex + 1) % NUM_SEGWIDTHS;
    prog = 0;
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

/**
 * wait: ms to wait between updates. Takes (256 * wait) ms for a full loop
 * segmentLength: number of values from Wheel to stretch across strip
 * progress:  intermediate value; should be 0 or return value from previous call.
 *            allows you to handle GPIO in between writes
 */

uint8_t rainbowSegment(uint8_t wait, uint8_t segmentLength, uint8_t progress) {
  uint16_t i;
  for(i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * segmentLength / strip.numPixels()) + progress) & 255));
  }
  strip.show();
  delay(wait);

  return progress + 1;
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
