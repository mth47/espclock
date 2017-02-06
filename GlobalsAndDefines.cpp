// *** General configuration settings ***

#include "GlobalsAndDefines.h"

Trace serialTrace;

// *** End of general configuration settings ***

CRTC Rtc;

/* 
 * ------------------------------------------------------------------------------
 * LED configuration
 * ------------------------------------------------------------------------------
 */

CRGB leds[NUM_LEDS];
CRGB leds_target[NUM_LEDS];

CClockDisplay clock;

/*
 * ------------------------------------------------------------------------------
 * Clock configuration/variables
 * ------------------------------------------------------------------------------
 */

uint8_t brightnessNight =  5;
uint8_t brightnessDay   = 50;
uint8_t brightness = brightnessDay;
uint8_t nightStart = 22;
uint8_t nightEnd = 6;
CClockDisplay::eDialect clockDialect = CClockDisplay::eD_Ossi;

/*
 * ------------------------------------------------------------------------------
 * Configuration parameters configured by the WiFiManager and stored in the FS
 * ------------------------------------------------------------------------------
 */
 
//The default values for the NTP server and the Blynk token, if there are different values in config.json, they are overwritten.
char ntp_server[50] = DEFAULT_NTP;
char blynk_token[33] = "YOUR_BLYNK_TOKEN";

bool IsConfigMode = false;
bool IsTestMode = false;

char station_name[50] = DEFAULT_STATIONNAME;


