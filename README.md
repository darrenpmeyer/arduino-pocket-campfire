# Arduino Magical Pocket Campfire

This is PlatformIO Arduino code for a [Magical Pocket Campfire][1] created for
the [Veracode][2] Hackathon "10 and Five Sevenths."

It simulates fire based on 3 different "heat map" color palettes:

[1]: https://youtu.be/coNhXWDEn00
[2]: https://veracode.com

# How to use

Wire the LED strip to the Ardunio and test it, then:

1. Get the [Atom editor][3]
2. Install the [PlatformIO plugin][4]
3. Acquire [FastLED][5]; make sure it's 3.1.3 or newer
4. Clone this project and open it from Atom/PlatformIO
5. If you're not using an Uno, change the `platformio.ini` file accordingly
6. If you're not using WS2812 lights on pin 1 of the Arduino, change the
   constants that are `#define`d near the top of `src/main.cpp`
7. Build and upload to the Arduino

[3]: https://atom.io
[4]: http://platformio.org
[5]: https://fastled.io
