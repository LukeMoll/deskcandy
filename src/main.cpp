#include "main.h"

#define _LED D9 // D9 ignored for ESP8266 - connect to "RX"

#define NUM_LEDS 100
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> leds(NUM_LEDS, _LED); 

// if ESP8266 can do HW SPI, that's dope but can't figure it out now. 
#define _SCK D5
#define _MOSI D7
#define _CS D8
#define _RST D6
#define _DC D1

// for the older looking one
U8X8_SSD1305_128X64_ADAFRUIT_4W_SW_SPI u8x8(_SCK, _MOSI, _CS, _DC, _RST);

#define _BUTTON D2

#define BRIGHTNESS 0.5f
#define HUE_WIDTH 0.4f
#define TICKLEN_MS 25
#define SEQLEN_MS 20000 
#define OLED_TIMEOUT_MS 10000

void setup() {
  Serial.begin(115200);
  leds.Begin();
  leds.Show();

  base_hues(NUM_LEDS, HUE_WIDTH);
  leds.Show();

  pinMode(_BUTTON, INPUT);

  u8x8.begin();
  oled_print();
}

void oled_print() {
  u8x8.setPowerSave(0);
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_chroma48medium8_r); // works? needs latest u8g2
  char buf[16];
  snprintf(buf,16,"%3d LEDs", NUM_LEDS);
  u8x8.drawString(0,0,buf);
  Serial.println(buf);
  snprintf(buf,16,"%.0f%% Hue", HUE_WIDTH*100);
  u8x8.drawString(0,1,buf);
  Serial.println(buf);
  snprintf(buf,16,"Tseq: %dms", SEQLEN_MS);
  u8x8.drawString(0,2,buf);
  Serial.println(buf);
  snprintf(buf,16,"Ttick: %dms", TICKLEN_MS);
  u8x8.drawString(0,3,buf);
  Serial.println(buf);
  snprintf(buf,16,"= %d ticks", SEQLEN_MS / TICKLEN_MS);
  u8x8.drawString(0,4,buf);
  Serial.println(buf);
  snprintf(buf,16,"h+= %f", (float) TICKLEN_MS / (float) SEQLEN_MS);
  u8x8.drawString(0,5,buf);
  Serial.println(buf);
}

void base_hues(const uint8_t L, float W) {
  for(uint8_t i=0; i < L; i++) {
    float hue = ((float) i * W) / (float) L;
    leds.SetPixelColor(i, HsbColor(hue - (int) hue, 1.0f, BRIGHTNESS));
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

unsigned long tick_millis;
void loop() {
  tick_millis = millis() + TICKLEN_MS;
  progress(SEQLEN_MS / TICKLEN_MS);
  leds.Show();

  if(digitalRead(_BUTTON) == LOW && !buttonPressed) {
    u8x8.setPowerSave(0);
    screen_millis = millis();
    buttonPressed = true;
  }
  else {buttonPressed = false;}
  if(millis() - screen_millis > OLED_TIMEOUT_MS) { // screen has been on for a minute
    u8x8.setPowerSave(1);
  }

  if(tick_millis > millis()) {
    delay(tick_millis - millis());
  }
}