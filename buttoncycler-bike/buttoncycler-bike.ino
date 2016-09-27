#include <FastLED.h>
// Our custom data type
//#include "LEDStrip.h"

// Data pin for a single button operation
//#define BUTTON_PIN 9 // Flora
#define BUTTON_PIN 3 // Bike box

// Data pin for the front LED strip
// This pin is shared between the Flora and the bike box
#define DATA_PIN_F 6
// Number of LEDs on the front LED strip
#define NUM_LEDS_F 60

// Data pin(s) for the two back LED strip(s)
// These are not present in the Flora. No need to change them.
#define DATA_PIN_B1 5
#define DATA_PIN_B2 4
// Number of LEDs on each of the back LED strip(s)
#define NUM_LEDS_B 22

CRGB front[NUM_LEDS_F];
CRGB back1[NUM_LEDS_B];
CRGB back2[NUM_LEDS_B];

int f_animation = 50;
int b_animation = 1;
int rainbow_color = 0;
uint8_t gHue = 0;
uint8_t cycle = 0;

enum {SteadyDim, GettingBrighter, GettingDimmerAgain};
int  PixelState[NUM_LEDS_F];
CRGB PixelColor[NUM_LEDS_F];

void setup() {
  // Initialize the LED strips
  FastLED.addLeds<NEOPIXEL, DATA_PIN_F>(front, NUM_LEDS_F);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_B1>(back1, NUM_LEDS_B);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_B2>(back2, NUM_LEDS_B);

  // Initialize the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(BUTTON_PIN, HIGH);
  memset(PixelState, sizeof(PixelState), SteadyDim); // initialize all the pixels to SteadyDim.
  memset(PixelColor, sizeof(PixelColor), CRGB::Black); // initialize all the pixels to SteadyDim.
}

//typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
//uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

unsigned long start_time;
unsigned long now;

#define NUM_F_ANIMATIONS 6
#define NUM_B_ANIMATIONS 3
int pattern = 0;
uint16_t led_count;
uint16_t fader;
uint8_t glitter;

void loop() {
  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_MILLISECONDS( 100 ) {
    cycle++;
    if (cycle == 3) {
      cycle = 0;
    }
  }
  // put your main code here, to run repeatedly:
  switch (f_animation) {
    case 1:
      fill_rainbow(front, NUM_LEDS_F, gHue, 5);
      addGlitter(front, NUM_LEDS_F, 80);
      break;
    case 2:
      theaterChase(front, NUM_LEDS_F, true);
      break;
    case 3:
      theaterChase(front, NUM_LEDS_F, false);
      break;
    case 4:
      TwinkleMapPixels(front, NUM_LEDS_F, true);
      break;
    case 5:
      TwinkleMapPixels(front, NUM_LEDS_F, false);
      break;
    case 6:
      bpm(front, NUM_LEDS_F);
      break;
    case 50:
      switch (pattern) {
        case 0:
          led_count = 0;
          pattern++;
          break;
        case 1:
          EVERY_N_MILLISECONDS(50) {
            if (led_count < NUM_LEDS_F) {
              led_count++;
            }
          }
          if (led_count == NUM_LEDS_F - 1) {
            start_time = millis();
          }
          fill_rainbow(front, led_count, gHue, 5);
          now = millis();
          if (now - start_time > 2000 && led_count == NUM_LEDS_F) {
            glitter = 1;
            pattern++;
          }
          break;
        case 2:
          EVERY_N_MILLISECONDS(20) {
            if (glitter < 100) {
              glitter++;
            }
          }
          now = millis();
          fill_rainbow(front, NUM_LEDS_F, gHue, 5);
          addGlitter(front, NUM_LEDS_F, glitter);
          fader = 1;
          if (glitter == 99) {
            start_time = millis();
          }
          if (now - start_time > 1000 && glitter == 100) {
            fader = 0;
            pattern++;
          }
          break;
        case 3:
          EVERY_N_MILLISECONDS(4) {
            if (fader < 255) {
              fader++;
            }
          }
          if (fader == 254) {
            start_time = millis();
          }
          now = millis();
          fill_rainbow(front, NUM_LEDS_F, gHue, 5);
          fadeToBlackBy(front, NUM_LEDS_F, fader);
          addGlitter(front, NUM_LEDS_F, glitter);
          if (now - start_time > 1000 && fader == 255) {
            pattern++;
          }
          break;
        case 4:
          EVERY_N_MILLISECONDS(10) {
            if (fader > 0 ) {
              fader--;
            }
          }
          if (fader != 0) {
            fadeToBlackBy(front, NUM_LEDS_F, fader);
          }
          if (fader == 1) {
            start_time = millis();
          }
          addGlitter(front, NUM_LEDS_F, glitter);
          now = millis();
          if (allColor(front, NUM_LEDS_F, CRGB::White) && fader == 0) {
            start_time = millis();
            pattern++;
          }
          break;
        case 5:
          now = millis();
          fadeToBlackBy(front, NUM_LEDS_F, 1);
          if (allColor(front, NUM_LEDS_F, CRGB::Black)) {
            start_time = millis();
            led_count = 0;
            pattern++;
          }
          break;
        case 6:
          EVERY_N_MILLISECONDS(50) {
            if (led_count < NUM_LEDS_F) {
              led_count++;
            }
          }
          bpm(front, led_count);
          break;
      }
      break;
    case 51:
      fadeToBlackBy(front, NUM_LEDS_F, 192);
      //      if (allBlack(front, NUM_LEDS_F) == true) {
      //        f_animation++;
      //      }
      break;
    case 53:
      //      runningTheaterChase(front, NUM_LEDS_F, true);
      break;
    default:
      fadeToBlackBy(front, NUM_LEDS_F, 5);
      break;
  }

  switch (b_animation) {
    case 1:
      fill_rainbow(back1, NUM_LEDS_B, gHue, 5);
      fill_rainbow(back2, NUM_LEDS_B, gHue, 5);
      break;
    case 2:
      theaterChase(back1, NUM_LEDS_B, true);
      theaterChase(back2, NUM_LEDS_B, true);
      break;
    case 3:
      theaterChase(back1, NUM_LEDS_B, false);
      theaterChase(back2, NUM_LEDS_B, false);
      break;
    default:
      fadeToBlackBy(back1, NUM_LEDS_B, 5);
      fadeToBlackBy(back2, NUM_LEDS_B, 5);
      break;
  }

  FastLED.show();
  buttons();
}

void bpm(CRGB* leds, uint8_t num_leds)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < num_leds; i++) {
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

bool allColor(CRGB* leds, uint8_t num_leds, CRGB color) {
  for (int i = 0; i < num_leds; i++) {
    if (leds[i] != color) {
      return false;
    }
  }
  return true;
}

void addGlitter(CRGB* leds, uint8_t num_leds, fract8 chanceOfGlitter) {
  if ( random8() < chanceOfGlitter) {
    leds[random16(num_leds)] += CRGB::White;
  }
}

void TwinkleMapPixels(CRGB* leds, uint8_t num_leds, bool white) {
  //  fadeToBlackBy( leds, num_leds, 20);
  random16_add_entropy(random());
  for ( uint16_t i = 0; i < num_leds; i++) {
    if ( PixelState[i] == SteadyDim) {
      // this pixels is currently: SteadyDim
      // so we randomly consider making it start getting brighter
      if ( random16(0, 200) < 1) {
        PixelState[i] = GettingBrighter;
        if (white) {
          PixelColor[i] = CRGB(4, 4, 4);
        } else {
          PixelColor[i] = CRGB(random16(0, 4), random16(0, 4), random16(0, 4));
        }
      }

    } else if ( PixelState[i] == GettingBrighter ) {
      // this pixels is currently: GettingBrighter
      // so if it's at peak color, switch it to getting dimmer again
      if ( leds[i].red >= 100 || leds[i].green >= 100 || leds[i].blue >= 100 ) {
        PixelState[i] = GettingDimmerAgain;
      } else {
        // otherwise, just keep brightening it:
        leds[i] += PixelColor[i];
      }

    } else { // getting dimmer again
      // this pixels is currently: GettingDimmerAgain
      // so if it's back to base color, switch it to steady dim
      if ( leds[i] <= CRGB(0, 0, 0) ) {
        leds[i] = CRGB(0, 0, 0); // reset to exact base color, in case we overshot
        PixelState[i] = SteadyDim;
      } else {
        // otherwise, just keep dimming it down:
        leds[i] -= CRGB(2, 2, 2);
      }
    }
  }
}

void runningTheaterChase(CRGB* leds, uint8_t num_leds, bool rainbow) {
  fadeToBlackBy( leds, num_leds, 50);
  for (int i = 0; i < num_leds; i = i + 3) {
    if (i + cycle < num_leds) {
      if (rainbow == true) {
        leds[i + cycle] = CHSV(gHue + i, 255, 192);
      } else {
        leds[i + cycle] = CRGB::White;
      }
    }
  }
}

void theaterChase(CRGB* leds, uint8_t num_leds, bool rainbow) {
  //  fill_solid( leds, num_leds, CRGB::Black);
  fadeToBlackBy( leds, num_leds, 50);
  for (int i = 0; i < num_leds; i = i + 3) {
    if (i + cycle < num_leds) {
      if (rainbow == true) {
        leds[i + cycle] = CHSV(gHue + i, 255, 192);
      } else {
        leds[i + cycle] = CRGB::White;
      }
    }
  }
}

void buttons() {
  int b = checkButton();
  if (b == 1) {
    f_animation++;
    if (f_animation > NUM_F_ANIMATIONS)
      f_animation = 1;
  }

  if (b == 2) {
    b_animation++;
    if (b_animation > NUM_B_ANIMATIONS)
      b_animation = 1;
  }
  if (b == 3) {
    f_animation = 50;
    b_animation = 50;
  }
  if (b == 4) {
    f_animation = 100;
    b_animation = 100;
  }
}


// Button timing variables
int debounce = 20; // ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 250; // max ms between clicks for a double click event
int holdTime = 2000; // ms hold period: how long to wait for press+hold event
int longHoldTime = 5000; // ms long hold period: how long to wait for press+hold event

// Other button variables
boolean buttonVal = HIGH; // value read from button
boolean buttonLast = HIGH; // buffered value of the button's previous state
boolean DCwaiting = false; // whether we're waiting for a double click (down)
boolean DConUp = false; // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true; // whether it's OK to do a single click
long downTime = -1; // time the button was pressed down
long upTime = -1; // time the button was released
boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false; // when held, whether to wait for the up event
boolean holdEventPast = false; // whether or not the hold event happened already
boolean longHoldEventPast = false;// whether or not the long hold event happened already

int checkButton()
{
  int event = 0;
  // Read the state of the button
  buttonVal = digitalRead(BUTTON_PIN);
  // Button pressed down
  if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce) {
    downTime = millis();
    ignoreUp = false;
    waitForUp = false;
    singleOK = true;
    holdEventPast = false;
    longHoldEventPast = false;
    if ((millis() - upTime) < DCgap && DConUp == false && DCwaiting == true) DConUp = true;
    else DConUp = false;
    DCwaiting = false;
  }
  // Button released
  else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce) {
    if (not ignoreUp) {
      upTime = millis();
      if (DConUp == false) DCwaiting = true;
      else {
        event = 2;
        DConUp = false;
        DCwaiting = false;
        singleOK = false;
      }
    }
  }
  // Test for normal click event: DCgap expired
  if ( buttonVal == HIGH && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true) {
    event = 1;
    DCwaiting = false;
  }
  // Test for hold
  if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
    // Trigger "normal" hold
    if (not holdEventPast) {
      event = 3;
      waitForUp = true;
      ignoreUp = true;
      DConUp = false;
      DCwaiting = false;
      //downTime = millis();
      holdEventPast = true;
    }
    // Trigger "long" hold
    if ((millis() - downTime) >= longHoldTime) {
      if (not longHoldEventPast) {
        event = 4;
        longHoldEventPast = true;
      }
    }
  }
  buttonLast = buttonVal;
  return event;
}
