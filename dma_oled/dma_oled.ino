#include <Arduino.h>
#include <NeoPixelBus.h>

#define _LED D9 // D9 ignored for ESP8266 - connect to "RX"

#define NUM_LEDS 35
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> leds(NUM_LEDS, _LED); 

#include <U8x8lib.h>

// if ESP8266 can do HW SPI, that's dope but can't figure it out now. 
#define _SCK D5
#define _MOSI D7
#define _CS D8
#define _RST D6
#define _DC D1

// for the older looking one
U8X8_SSD1305_128X64_ADAFRUIT_4W_SW_SPI u8x8(_SCK, _MOSI, _CS, _DC, _RST);

#define _BUTTON D2

#define HUE_WIDTH 0.4f
#define TICKLEN_MS 25
#define SEQLEN_MS 10000 

void setup() {
  Serial.begin(9600);
  leds.Begin();
  leds.Show();

  base_hues(NUM_LEDS, HUE_WIDTH);
  leds.Show();

  pinMode(_BUTTON, INPUT);

  u8x8.begin();

  // all of this should probably go in a fn
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r); // works? needs latest u8g2
  char buf[16];
  snprintf(buf,16,"%3d LEDs", NUM_LEDS);
  u8x8.drawString(0,0,buf);
  snprintf(buf,16,"%.0f%% Hue", HUE_WIDTH*100);
  u8x8.drawString(0,1,buf);
  snprintf(buf,16,"Tseq: %dms", SEQLEN_MS);
  u8x8.drawString(0,2,buf);
  snprintf(buf,16,"Ttick: %dms", TICKLEN_MS);
  u8x8.drawString(0,3,buf);
  snprintf(buf,16,"= %d ticks", SEQLEN_MS / TICKLEN_MS);
  u8x8.drawString(0,4,buf);
  
}

void base_hues(const uint8_t L, float W) {
  for(uint8_t i=0; i < L; i++) {
    float hue = ((float) i * W) / (float) L;
    leds.SetPixelColor(i, HsbColor(hue - (int) hue, 1.0f, 0.8f));
  }
}

void progress(int16_t num_ticks) {
  for(uint8_t i=0; i < NUM_LEDS; i++) {
    HsbColor current = HsbColor(leds.GetPixelColor(i)); 
    // we might be able to remove this if we assert 
    // that it always returns the HsbColor we set in base_hues

    current.H += 1 / (float) num_ticks;
    if(current.H > 1) {
      current.H -= (int) current.H;
    }
    leds.SetPixelColor(i, current);
  }
}

bool buttonPressed = false;
unsigned long screen_millis = millis();

void loop() {
  progress(SEQLEN_MS / TICKLEN_MS);
  leds.Show();
  delay(TICKLEN_MS);
  
  if(digitalRead(_BUTTON) == LOW && !buttonPressed) {
    u8x8.setPowerSave(0);
    screen_millis = millis();
  }
  if(millis() - screen_millis > 60000) { // screen has been on for a minute
    u8x8.setPowerSave(1);
  }
}
