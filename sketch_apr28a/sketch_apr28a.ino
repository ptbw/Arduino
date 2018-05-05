#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 64

Adafruit_NeoPixel pixels(NUM_PIXELS, D8, NEO_GRB | NEO_KHZ800);

void setup() {
  pixels.begin();
}

void setColor(uint32_t color) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(50);
  }
}

void loop() {
  setColor(pixels.Color(100, 0, 0));
  delay(1000);
  setColor(pixels.Color(0, 100, 0));
  delay(1000);
  setColor(pixels.Color(0, 0, 100));
  delay(1000);
}
