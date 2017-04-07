#include "Arduino.h"
#include "FastLED.h"

#define LED_COUNT 30
#define LED_DRIVER WS2812
#define LED_COLOR_ORDER GRB
#define LED_DATA_PIN 1

#define COOLING 55   // smaller = taller flames
#define SPARKING 180 // smaller = more flickery
#define SPLIT 10 // lights per leg

CRGB led[LED_COUNT];
byte heat[LED_COUNT];
CRGBPalette16 currentPal;

extern const TProgmemRGBPalette16 HeatGreen_p FL_PROGMEM =
{
    0x000000,
    0x003300, 0x006600, 0x009900, 0x00CC00, 0x00FF00,
    0x33FF00, 0x66FF00, 0x99FF00, 0xCCFF00, 0xFFFF00,
    0xFFFF33, 0xFFFF66, 0xFFFF99, 0xFFFFCC, 0xFFFFFF
};

extern const TProgmemRGBPalette16 HeatBlue_p FL_PROGMEM =
{
    0x000000,
    0x000033, 0x000066, 0x000099, 0x0000CC, 0x0000FF,
    0x3300FF, 0x6600FF, 0x9900FF, 0xCC00FF, 0xFF00FF,
    0xFF33FF, 0xFF66FF, 0xFF99FF, 0xFFCCFF, 0xFFFFFF
};


/* I borrowed _very heavily_ from a FastLED example for this function:
   https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
*/
void FlameTick(byte base, byte count) {
  // cool the pixels
  byte num_leds = base+count;
  for( int i=base; i < base+count; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / count) + 2));
  }

  // drift and cool
  for( int k=num_leds-1; k >= base+2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // spark
  if( random8() < SPARKING ) {
    int y = random8(3) + base;
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // map
  for ( int j=base; j < num_leds; j++ ) {
    byte colorindex = scale8( heat[j], 240 ); // better for palette use
    led[j] = ColorFromPalette(currentPal, colorindex);
  }
}

void setup() {
  delay(800);  // short boot delay to allow for resets/uploads
  FastLED.addLeds<LED_DRIVER, LED_DATA_PIN, LED_COLOR_ORDER>(led, LED_COUNT);
  currentPal = HeatGreen_p;
  FastLED.show();
}

void loop() {
  for (byte f=0; f < LED_COUNT; f+=SPLIT) {
    if (f+SPLIT > LED_COUNT) { }
    else {
      FlameTick(f, SPLIT);
    }
  }
  // FlameTick(0, 10);

  FastLED.delay(15);
  FastLED.show();
}
