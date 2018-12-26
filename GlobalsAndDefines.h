// *** General configuration settings ***

#ifndef _GAD_H
#define _GAD_H

#include <FS.h>                   //From the WifiManager AutoConnectWithFSParameters: this needs to be first, or it all crashes and burns... hmmm is this really the case?


// Enable to configure LEDs for small clock, otherwise big clock
#define SMALLCLOCK
// Enable to control all LEDs indidually via OPC, e.g. with Processing IDE (PC or cell phone app) and OPC libarary, opens port 7890
// #define ENABLE_OPC
// Enable Blynk to control the clock via Blynk Handy App, bynk account needed, connects to blynk-cloud.com:8442           
// #define ENABLE_BLYNK
// Enable local WebServer
#define ENABLE_LOCAL_WEBSERVER

// Enables Serial Tracing
#define TRACE
// Enables Debug Traces, in VS this define is set by project settings in Arduino IDE not
//#define _DEBUG


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

extern CRGB color_WarmWhite;
extern CRGB color_WarmWhite_sr_start;
extern CRGB color_WarmWhite_sr_middle;
extern CRGB nightColor;
extern CRGB dayColor;

extern CClockDisplay clock;

/*
 * ------------------------------------------------------------------------------
 * Clock configuration/variables/methods
 * ------------------------------------------------------------------------------
 */
#define DEFAULT_NIGHT_START (uint16_t) 22 * 60
#define DEFAULT_NIGHT_END (uint16_t) 6 * 60

#define DEFAULT_TV_START (uint16_t) 20 * 60 + 15
#define DEFAULT_TV_END (uint16_t) 23 * 60 + 45


//#ifdef SMALLCLOCK
    #define BRIGHTNESS_TVSIM 100
//#else
//    #define BRIGHTNESS_TVSIM 80
//#endif
#define BRIGHTNESS_TEST 5

extern uint8_t brightnessNight;
extern uint8_t brightnessDay;
extern uint8_t brightness;
extern uint16_t nightStart;
extern uint16_t nightEnd;

extern uint16_t tvStart;
extern uint16_t tvEnd;

extern bool bSunRise;
extern bool bRunSunRise;
// ONE_SUNRISE_STEP_IN_MS * NUM_SUNRISE_STEPS == overal time from zero to max brightness
// 5 min == 5 x 60 x 1000 milli seconds
// brightness can be adjusted from 0 to 100 only
#define ONE_SUNRISE_STEP_IN_MS 2000
#define NUM_SUNRISE_STEPS 100

extern unsigned long holdTime;

extern bool bIsDay;

uint16_t GetMinOfTime(int8_t hour, int8_t min);
uint16_t GetMinOfTime(time_t t);
uint8_t GetHourOfDayMin(uint16_t m);
uint8_t GetMinOfDayMin(uint16_t m);
bool IsValidHour(uint8_t h);
bool IsValidMin(uint8_t m);
bool IsValidDayMin(uint16_t m);
// requires nightStart and nightEnd in minutes of the day
bool IsNight(time_t local);
// requires tvStart and tvEnd in minutes of the day
bool IsTvSimTime(time_t local);
bool IsStillSunriseTime(time_t local);

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

enum ClockMode
{
    eCM_clock = 0,
    eCM_test = 1,
    eCM_tv = 2,
    eCM_tv_auto = 3,
    eCM_opc = 4,

};

extern ClockMode clockMode;

#ifdef SMALLCLOCK
  #define DEFAULT_STATIONNAME "wordclock_small"
#else
  #define DEFAULT_STATIONNAME "wordclock_big"
#endif

extern char station_name[50];


char* ftoa(double f, int precision);


#endif


