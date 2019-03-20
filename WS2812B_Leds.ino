// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "FastLED.h"          // LED library
#include <DS3231.h>           // Real time clock
#include <Wire.h>             // i2c wire protocol
#include <Time.h>             // Time Library
#include <TinyGPS++.h>        // GPS Library
#include <SoftwareSerial.h>

#define LED_DATA_PIN 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 300
#define BRIGHTNESS 96
#define GPS_BAUD 4800
#define GPS_RX_PIN 10
#define GPS_TX_PIN 9

CRGB leds[NUM_LEDS];

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);

RTClib RTC;
DS3231 Clock;
byte lastSecond = 255;

void display(char* text, CRGB* colors) {
  Serial.print("Current time: ");
  Serial.print(text);
  Serial.println();

  #define digits 6
  #define sheetsPerDigit 10
  #define ledsPerDigitSheet 2
  
  #define separators 2
  #define sheetsPerSeparator 2
  #define ledsPerSeparatorSheet 1

  int totalLEDs = digits * sheetsPerDigit * ledsPerDigitSheet +
    separators * sheetsPerSeparator * ledsPerSeparatorSheet;

  for (int ledIndex = 0; ledIndex < NUM_LEDS; ledIndex++)
    leds[ledIndex] = CRGB::Black;

  int firstLED = 0;
  for (int characterIndex = 0; characterIndex < digits + separators; characterIndex++) {
    char character = text[characterIndex];
    int ledsPerCharacter;
    int ledsPerSheet;
    switch (characterIndex) {
      case 0:
      case 1:
      case 3:
      case 4:
      case 6:
      case 7:
        ledsPerSheet = ledsPerDigitSheet;
        ledsPerCharacter = ledsPerDigitSheet * sheetsPerDigit;
        break;
      case 2:
      case 5:
        ledsPerSheet = ledsPerSeparatorSheet;
        ledsPerCharacter = ledsPerSeparatorSheet * sheetsPerSeparator;
        break;
    }

    int sheet = 0;
    switch (character) {
      case ' ':
        // Skip ahead to the next character
        continue;
      case '0':
        sheet = 0;
        break;
      case '1':
        sheet = 1;
        break;
      case '2':
        sheet = 2;
        break;
      case '3':
        sheet = 3;
        break;
      case '4':
        sheet = 4;
        break;
      case '5':
        sheet = 5;
        break;
      case '6':
        sheet = 6;
        break;
      case '7':
        sheet = 7;
        break;
      case '8':
        sheet = 8;
        break;
      case '9':
        sheet = 9;
        break;
      case '.':
        sheet = 0;
        break;
      case '°':
        sheet = 1;
        break;
      case ':':
        sheet = 0;
        // Then we also need to turn on sheet 1
        break;
      default:
        Serial.print("Unable to display character ");
        Serial.print(character);
        Serial.print(' at position ');
        Serial.print(characterIndex);
        Serial.println();
        Serial.flush();
        return;
    }

    for (int i = 0; i < ledsPerSheet; i++) {
      int ledIndex = firstLED + ledsPerSheet * sheet + i;
      leds[ledIndex] = colors[characterIndex];
    }

    if (character == ':') {
      // Also do sheet 1 for ':'
      for (int i = 0; i < ledsPerSheet; i++) {
        int ledIndex = firstLED + ledsPerSheet * sheet + i;
        leds[firstLED + ledsPerSheet * 1 + i] = colors[characterIndex];
      }
    }

    firstLED += ledsPerCharacter;
  }

  FastLED.show();
}

void setup() {
  Wire.begin(); // Start i2c
  Serial.begin(9600); // Start serial
  ss.begin(GPS_BAUD);

  delay(500); // initial delay of a few seconds is recommended
  initLEDs();
}

void initLEDs() {
  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
}

int showTimeHue = 0;
void showTime() {
  showTimeHue += 8;
  showTimeHue = showTimeHue % 256;

  DateTime now = RTC.now();
  char text[8];
  CRGB colors[8];

  for (int hue = 0; hue < 8; hue++) {
    colors[hue] = CHSV(showTimeHue + hue * 5, 255, 255);
  }

  byte hour = now.hour() % 12;
  if (hour == 0) hour = 12;
  snprintf(text, 8, "%02d", hour);
  text[2] = ':';
  snprintf(text + 3, 8, "%02d", now.minute());
  text[5] = ':';
  snprintf(text + 6, 8, "%02d", now.second());
  display(text, colors);
}

// main program
void loop() {
  showTime();
  delay(1000);
}
