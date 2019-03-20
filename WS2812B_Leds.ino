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
  turnLEDsOff();
}

void outputGPSInfo() {
    while (ss.available() > 0)
    if (!gps.encode(ss.read()))
      return;
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();

}

void outputRealTimeClockInfo() {
  DateTime now = RTC.now();
  int currentSecond = now.second();
  if (currentSecond == lastSecond) {
    return;
  }
  lastSecond = currentSecond;

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print("Temp = ");
  Serial.println(Clock.getTemperature(), 2);
}

// switches off all LEDs
void turnLEDsOff() {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

int hue = 0;
void shiftPixels(long delayTime) {
  for (int i = NUM_LEDS; i > 0; i--) {
    leds[i] = leds[i - 1];
  }
  hue += 1;
  CRGB newPixel = CHSV(hue, 255, 255);
  leds[0] = newPixel;
  FastLED.show();
}

// main program
void loop() {
  outputRealTimeClockInfo();
  shiftPixels(10);
}
