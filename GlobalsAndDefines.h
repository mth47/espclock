// *** General configuration settings ***

#ifndef _GAD_H
#define _GAD_H

#include <FS.h>                   //From the WifiManager AutoConnectWithFSParameters: this needs to be first, or it all crashes and burns... hmmm is this really the case?


// Enable to configure LEDs for small clock, otherwise big clock
#define SMALLCLOCK
// Enable to control all LEDs indidually via OPC, e.g. with Processing IDE (PC or cell phone app) and OPC libarary, opens port 7890
// #define ENABLE_OPC
// Enable Blync to control the clock via Blync Handy App, bynk account needed, connects to blync-cloud.com:8442           
// #define ENABLE_BLYNK
// Enable local WebServer
#define ENABLE_LOCAL_WEBSERVER

// Enables Serial Tracing
#define TRACE
// Enables Debug Traces
// #define _DEBUG

#include "Trace.h"
extern Trace serialTrace;

#define FASTLED_ESP8266_RAW_PIN_ORDER

// *** End of general configuration settings ***

// time stuff
#include <TimeLib.h>              //http://www.arduino.cc/playground/Code/Time
#include "CRTC.h"
#include "CNTPClient.h"
#include <Timezone.h>             //https://github.com/JChristensen/Timezone (Use https://github.com/willjoha/Timezone as long as https://github.com/JChristensen/Timezone/pull/8 is not merged.)
extern CRTC Rtc;

/* 
 * ------------------------------------------------------------------------------
 * LED configuration
 * ------------------------------------------------------------------------------
 */
#include "CClockDisplay.h"

// Number of LEDs used for the clock 
// small 25x25 (wiring: 11x10 + 2 spare LEDs + 4 minute LEDs + 2 spare LEDs)
// big 50x50 (wiring: 11x10 + 1 minute LED + 8 spare LEDs + 3 minute LEDs )
#ifdef SMALLCLOCK
  #define NUM_LEDS 118
  #define CLOCKSETUP setup_small
#else
  #define NUM_LEDS 122
  #define CLOCKSETUP setup
#endif

// DATA_PIN and CLOCK_PIN used by FastLED to control the APA102C LED strip. 
#define DATA_PIN 13
#define CLOCK_PIN 14

extern CRGB leds[NUM_LEDS];
extern CRGB leds_target[NUM_LEDS];


// default is the best warm white found so far
#define DEFAULT_RED 255
#define DEFAULT_GREEN 127
#define DEFAULT_BLUE 36

extern CClockDisplay clock;

/*
 * ------------------------------------------------------------------------------
 * Clock configuration/variables
 * ------------------------------------------------------------------------------
 */

extern uint8_t brightnessNight;
extern uint8_t brightnessDay;
extern uint8_t brightness;
extern uint8_t nightStart;
extern uint8_t nightEnd;
extern CClockDisplay::eDialect clockDialect;

/*
 * ------------------------------------------------------------------------------
 * Configuration parameters configured by the WiFiManager and stored in the FS
 * ------------------------------------------------------------------------------
 */
 
//The default values for the NTP server and the Blynk token, if there are different values in config.json, they are overwritten.
#define DEFAULT_NTP "0.de.pool.ntp.org"
extern char ntp_server[50];
extern char blynk_token[33];
extern bool IsConfigMode;
extern bool IsTestMode;

#ifdef SMALLCLOCK
  #define DEFAULT_STATIONNAME "wordclock_small"
#else
  #define DEFAULT_STATIONNAME "wordclock_big"
#endif

extern char station_name[50];

#endif


