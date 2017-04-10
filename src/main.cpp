#include "Arduino.h"
#include "FastLED.h"
#include "palette.h"

#define LED_COUNT 9          // total number of LEDs
#define LED_DRIVER WS2812    // FastLED chipset to use
#define LED_COLOR_ORDER GRB  // Color order: RGB or GRB usually
#define LED_DATA_PIN 1       // which Ardunio digital pin for LED clock + data

#define COOLING 60    // smaller = taller flames
#define COOL_STAGE 18 // separation between intensity stages
#define SPARKING 180  // smaller = more flickery
#define SPLIT 9       // lights per leg
#define FPS 30        // frames per second, approx.
#define REVERSE false // reverse the flame direction

CRGB led[LED_COUNT];
uint8_t heat[LED_COUNT];
CRGBPalette16 palettes[3] = { HeatColors_p, HeatGreen_p, HeatBlue_p };
uint8_t stage = 0;
CRGBPalette16 currentPal;

/* I borrowed _very heavily_ from a FastLED example for this function:
   https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
*/
void FlameTick(uint8_t base, uint8_t count, bool reverse) {
  // cool the pixels
  uint8_t num_leds = base+count;
  uint8_t cooling = COOLING + (COOL_STAGE * stage);
  for( uint8_t i=base; i < base+count; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / count) + 2));
  }

  // drift and cool
  for( uint8_t k=num_leds-1; k >= base+2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // spark
  if( random8() < SPARKING ) {
    int y = random8(3) + base;
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // map
  for ( uint8_t j=base; j < num_leds; j++ ) {
    uint8_t pixelnumber = j;
    if( reverse ) { pixelnumber = (num_leds-1) - j; }
    uint8_t colorindex = scale8( heat[pixelnumber], 240 ); // better for palette use
    led[pixelnumber] = ColorFromPalette(currentPal, colorindex);
  }
}

void setup() {
  delay(800);  // short boot delay to allow for resets/uploads
  FastLED.addLeds<LED_DRIVER, LED_DATA_PIN, LED_COLOR_ORDER>(led, LED_COUNT);
  currentPal = palettes[0];
  FastLED.setBrightness(128);
  FastLED.show();
}

void loop() {
  static uint32_t stage_ticks = 0;
  static uint32_t color_ticks = 0;
  static uint8_t palette_index = 0;

  // randomly change the stage of intensity
  if (stage_ticks > (500 / FPS)) {
    // AT least half a second has elapsed since resets
    if ((random8(0,100) < 20) || (stage_ticks > 10000 / FPS)) {
      // 20% chance + guaranteed if it's been longer than 10s without a change
      stage = random8(4);
      stage_ticks = 0;
    }
  }

  //randomly change the color palette
  if (palette_index == 0) {
    // default palette on, lower chance of change
    if (color_ticks > 10000 / FPS) {
      // only change if it's been at least 10s
      if ((random8(0,100) < 5) || (color_ticks > 90000 / FPS)) {
        // 5% chance + guaranteed if it's been longer than 90s without a change
        palette_index = random8(1,3);
        color_ticks = 0;
      }
    }
  }
  else {
    // higher chance of change, since it's not the default palette
    if (color_ticks > 3000 / FPS) {
      // only change if it's been at least 3s
      if ((random8(0,100) < 30) || (color_ticks > 15000 / FPS)) {
        // 30% chance + guaranteeed if it's been longer than 15s without change
        palette_index = 0;
        color_ticks = 0;
      }
    }
  }

  currentPal = palettes[palette_index];

  // build the next frame for each column of fire
  for (uint8_t f=0; f < LED_COUNT; f+=SPLIT) {
    if (f+SPLIT <= LED_COUNT) { FlameTick(f, SPLIT, REVERSE); }
  }

  FastLED.delay( 1000 / FPS );
  FastLED.show();
  stage_ticks++;
  color_ticks++;
}
