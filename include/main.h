#include <Arduino.h>
#include <NeoPixelBus.h>
#include <U8x8lib.h>

void oled_print();
void base_hues(const uint8_t L, float W);
void progress(int16_t num_ticks);
int check_hue_advance();
