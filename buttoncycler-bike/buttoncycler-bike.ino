#include <FastLED.h>

// Data pin for a single button operation
//#define BUTTON_PIN 9 // Flora
#define BUTTON_PIN 3 // Bike box

// Data pin for the front LED strip
#define DATA_PIN_F 6
// Number of LEDs on the front LED strip
#define NUM_LEDS_F 60

// Data pin(s) for the back LED strip(s)
#define DATA_PIN_B1 5
#define DATA_PIN_B2 4
// Number of LEDs on the back LED strip(s)
#define NUM_LEDS_B 22

CRGB front[NUM_LEDS_F];
CRGB back1[NUM_LEDS_B];
CRGB back2[NUM_LEDS_B];

void setup() {
  // Initialize the LED strips
  FastLED.addLeds<NEOPIXEL, DATA_PIN_F>(front, NUM_LEDS_F);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_B1>(back1, NUM_LEDS_B);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_B2>(back2, NUM_LEDS_B);

  // Initialize the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(BUTTON_PIN, HIGH);
}


int f_animation = 1;
int b_animation = 1;
int rainbow_color = 0;
uint8_t gHue = 0;
uint8_t cycle = 0;

#define NUM_F_ANIMATIONS 3
#define NUM_B_ANIMATIONS 3

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
      break;
    case 2:
      theaterChase(front, NUM_LEDS_F, true);
      break;
    case 3:
      theaterChase(front, NUM_LEDS_F, false);
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

void theaterChase(CRGB* leds, uint8_t num_leds, bool rainbow) {
  for (int i = 0; i < num_leds; i = i + 3) {
    if (i + cycle < num_leds) {
      if (rainbow == true) {
        leds[i + cycle] = CHSV(gHue + i, 255, 192);
      } else {
        leds[i + cycle] = CRGB::White;
      }
    }
    if (i + cycle - 1 >= 0 && i + cycle - 1 < num_leds ) {
      leds[i + cycle - 1] = CRGB::Black;
    }
    if (i + cycle - 2 >= 0 && i + cycle - 2 < num_leds ) {
      leds[i + cycle - 2] = CRGB::Black;
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

  if (b == 3 || b == 4) {
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
