#include <Adafruit_NeoPixel.h>

#define LEDS 30
#define STRIP_PIN D2
#define SWITCH_PIN D5

struct RGB {
  int red;
  int green;
  int blue;
};

RGB red = { 255, 0, 0 };
RGB green = { 0, 255, 0 };
RGB blue = { 0, 0, 255 };
RGB white = { 255, 255, 255 };
RGB off = { 0, 0, 0 };

int STATE_UP = 1;
int STATE_DOWN = 0;

int twinklesValue[LEDS];
int twinklesState[LEDS];
int twinklesColor[LEDS];
int newTwinkleCounter = 0;

bool initializedPoliceLights = false;

bool switchPressed = false;
int LIGHTS_MODE_TWINKLE_WHITE = 0;
int LIGHTS_MODE_TWINKLE_RGB = 1;
int LIGHTS_MODE_POLICE = 2;
int currentMode = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  strip.setBrightness(50);
  strip.begin();
  strip.show();

  for (int led = 0; led < LEDS; led++) {
    twinklesValue[led] = 0;
    twinklesState[led] = STATE_DOWN;
    twinklesColor[led] = 0;
  }
}

void loop() {
  int switchDown = digitalRead(SWITCH_PIN);

  if (switchDown == 0 && switchPressed == false) {
    switchPressed = true;
  } else if (switchDown == 1 && switchPressed == true) {
    currentMode = (currentMode + 1) % 3;
    switchPressed = false;

    initializedPoliceLights = false;
    for (int led = 0; led < LEDS; led++) {
      strip.setPixelColor(led, strip.Color(0, 0, 0));
    }
  }


  if (currentMode == LIGHTS_MODE_TWINKLE_WHITE) {
    strip.setBrightness(200);
    twinkle(false);  // white lights
  } else if (currentMode == LIGHTS_MODE_TWINKLE_RGB) {
    strip.setBrightness(255);
    twinkle(true);  // RGB lights
  } else if (currentMode == LIGHTS_MODE_POLICE) {
    strip.setBrightness(150);
    policeLights();
  }
}

void twinkle(bool rgbMode) {
  if (newTwinkleCounter == 0) {
    int newTwinkleLed = getNewRandomTwinkleLed();
    int newTwinkleColor = random(0, 3);
    twinklesState[newTwinkleLed] = STATE_UP;
    twinklesColor[newTwinkleLed] = newTwinkleColor;
    newTwinkleCounter = 100;
  }
  newTwinkleCounter--;

  for (int led = 0; led < LEDS; led++) {
    int twinkleValue = twinklesValue[led];
    int twinkleState = twinklesState[led];
    if (twinkleValue > 0 || twinkleState == STATE_UP) {
      if (twinkleState == STATE_DOWN) {
        twinkleValue--;
      } else {
        twinkleValue++;
      }

      if (twinkleValue == 255) {
        twinklesState[led] = STATE_DOWN;
      }

      twinklesValue[led] = twinkleValue;
      if (rgbMode) {
        int red = 0;
        int green = 0;
        int blue = 0;

        int color = twinklesColor[led];
        if (color == 0) {
          red = twinkleValue;
        } else if (color == 1) {
          green = twinkleValue;
        } else {
          blue = twinkleValue;
        }
        strip.setPixelColor(led, strip.Color(red, green, blue));
      } else {
        strip.setPixelColor(led, strip.Color(twinkleValue, twinkleValue, twinkleValue));
      }
    }
  }
  strip.show();
  delay(2);
}

int getNewRandomTwinkleLed() {
  while (true) {
    int newTwinkleLed = random(0, LEDS);
    if (twinklesValue[newTwinkleLed] == 0) {
      return newTwinkleLed;
    }
  }
  return 0;
}

void policeLights() {
  if (initializedPoliceLights == false) {
    initializedPoliceLights = true;
    for (int led = 0; led < LEDS / 2; led++) {
      setStripPixelColor(red, led);
    }
    for (int led = LEDS / 2; led < LEDS; led++) {
      setStripPixelColor(blue, led);
    }
  }

  int breakPixel = 0;
  for (int led = 0; led < LEDS; led++) {
    int secondBreakPixel = getSecondBreakPixel(breakPixel);
    setStripPixelColor(blue, breakPixel);
    setStripPixelColor(red, secondBreakPixel);
    breakPixel++;
    delay(30);
    strip.show();
  }
}

int getSecondBreakPixel(int breakPixel) {
  return ((LEDS / 2) + breakPixel) % LEDS;
}

void setStripPixelColor(RGB color, int led) {
  if (led < 0 || led >= LEDS) return;
  strip.setPixelColor(led, color.red, color.green, color.blue);
}
