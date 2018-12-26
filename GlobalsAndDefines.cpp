// *** General configuration settings ***

#include "GlobalsAndDefines.h"

Trace serialTrace;

// *** End of general configuration settings ***

CRTC Rtc;

/*
   ------------------------------------------------------------------------------
   LED configuration
   ------------------------------------------------------------------------------
*/

CRGB leds[NUM_LEDS];
CRGB leds_target[NUM_LEDS];

CClockDisplay clock;

CRGB color_WarmWhite(DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE);
CRGB color_WarmWhite_sr_start(230, 90, 25);

CRGB nightColor(DEFAULT_RED, 0, 0);
CRGB dayColor(DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE);

/*
   ------------------------------------------------------------------------------
   Clock configuration/variables/methods
   ------------------------------------------------------------------------------
*/

uint8_t brightnessNight =  5;
uint8_t brightnessDay   = 50;
uint8_t brightness = brightnessDay;
uint16_t nightStart = DEFAULT_NIGHT_START;
uint16_t nightEnd = DEFAULT_NIGHT_END;

uint16_t tvStart = DEFAULT_TV_START;
uint16_t tvEnd = DEFAULT_TV_END;

#define TV_RAND_TIME (uint16_t) 4 * 60 // 4 am
//#define TV_RAND_TIME (uint16_t) 13*60+18
#define MAX_TV_START_VARIATION (uint16_t) 12
#define MAX_TV_END_VARIATION (uint16_t) 19 
#define MIDNIGHT (uint16_t) 24 * 60
short tvVari1 = 0;
short tvVari2 = 0;

bool bSunRise = false;
bool bRunSunRise = false;

unsigned long holdTime = 0;

bool bIsDay = true;

uint16_t GetMinOfTime(int8_t hour, int8_t min)
{
  if (!IsValidHour(hour))
    hour = 0;

  if (!IsValidMin(min))
    min = 0;

  return (hour * 60) + min;
}

uint16_t GetMinOfTime(time_t t)
{
  return (hour(t) * 60) + minute(t);
}

uint8_t GetHourOfDayMin(uint16_t m)
{
  if (m < 0)
    return 0;

  return m / 60;  
}

uint8_t GetMinOfDayMin(uint16_t m)
{
  if (m < 0)
    return 0;

  return m % 60;  
}

bool IsValidHour(uint8_t h)
{  
  if (0 <= h && 23 >= h)
    return true;

  return false;  
}

bool IsValidMin(uint8_t min)
{
  if (0 <= min && 59 >= min)
    return true;

  return false;
}

bool IsValidDayMin(uint16_t m)
{
  if(IsValidHour(GetHourOfDayMin(m)) && IsValidMin(GetMinOfDayMin(m)))
    return true;

  return false;
}

bool IsWithInTime(time_t local, uint16_t tStart, uint16_t tEnd, bool& bIsWithin, const char* functionLabel)
{
    if (tStart == tEnd)
    {
        if (bIsWithin)
            serialTrace.Log(T_DEBUG, "%s: Start == End '%d', return false", functionLabel, tEnd);


        return (bIsWithin = false);
    }

    int16_t currMin = GetMinOfTime(local);

    if (tEnd > tStart)
    {
        // the night starts after midnight and and ends in the morning
        if (currMin >= tStart && currMin < tEnd)
        {
            if (!bIsWithin)
                serialTrace.Log(T_DEBUG, "%s: currMin >= Start '%d' && currMin < End '%d', return true", functionLabel, tStart, tEnd);

            return (bIsWithin = true);
        }

        if (bIsWithin)
            serialTrace.Log(T_DEBUG, "%s: End > Start '%d', End '%d', return false", functionLabel, tStart, tEnd);

        return (bIsWithin = false);
    }
    else
    {
        // the night starts before midnight and and ends in the morning
        if (currMin >= tStart || currMin < tEnd)
        {
            if (!bIsWithin)
                serialTrace.Log(T_DEBUG, "%s: currMin >= Start '%d' ||  currMin < End '%d', return true", functionLabel, tStart, tEnd);

            return (bIsWithin = true);
        }
    }

    if (bIsWithin)
        serialTrace.Log(T_DEBUG, "%s: Start > End '%d', End '%d', return false", functionLabel, tStart, tEnd);

    return (bIsWithin = false);
}

bool IsNight(time_t local)
{
  static bool bNight = false;

  return IsWithInTime(local, nightStart, nightEnd, bNight, "IsNight");
} 

bool IsTvSimTime(time_t local)
{
    static bool bTvSim = false;
    static bool tv_rand_done = false; // prevents to calc the random minutes several time within one minute

    if (tvStart == tvEnd)
    {
        // tvSim is not enabled.
        return false;
    }

    // ok, now lets calc some random number of minutes and put them on top
    // we do this every night at 1am
    if(!tv_rand_done && TV_RAND_TIME == GetMinOfTime(local))
    { 
        uint16_t maxVari = 0;
        if(tvEnd - tvStart > 0)
            maxVari = tvEnd - tvStart;
        else
            maxVari = MIDNIGHT - tvStart + tvEnd;
        
        tv_rand_done = true;

        if (maxVari >= 2)
        {
            tvVari1 = random(1, (maxVari < MAX_TV_START_VARIATION + MAX_TV_END_VARIATION) ? maxVari / 2 : MAX_TV_START_VARIATION);
            // plus or minus?
            if (1 == random(1, 3))
                tvVari1 *= -1;

            tvVari2 = random(1, (maxVari < MAX_TV_START_VARIATION + MAX_TV_END_VARIATION) ? maxVari / 2 : MAX_TV_END_VARIATION);
            // plus or minus?
            if (1 == random(1, 3))
                tvVari2 *= -1;

            serialTrace.Log(T_DEBUG, "IsTvSimTime -  Calculated start variation '%d', Calculated end variation '%d'. Set daily variation time calculation flag.", tvVari1, tvVari2);
        }
        else
        {
            tvVari1 = 0;
            tvVari2 = 0;
        }
    }

    // reset the 
    if (tv_rand_done && TV_RAND_TIME + 5 == GetMinOfTime(local))
    {
        tv_rand_done = false;
        serialTrace.Log(T_DEBUG, "IsTvSimTime -  Reset daily variation time calculation flag.");
    }
    
    return IsWithInTime(local, tvStart + tvVari1, tvEnd + tvVari2, bTvSim, "IsTvSimTime");
    
}

// note this method is only used to detect the start time, so it is good enough to check for two minutes range, not three
// actually three is too much, we need one interval, e.g. second less, but do not compare seconds.
bool IsStillSunriseTime(time_t local)
{
    //serialTrace.Log(T_INFO, "IsStillSunriseTime: GetMinOfTime '%d', nightEnd '%d', sunrise end '%d'", GetMinOfTime(local), nightEnd, ((ONE_SUNRISE_STEP_IN_MS / 1000 * NUM_SUNRISE_STEPS) / 60));
    // we have the time in MINUTES!!!, so can simply compare with the max milli seconds the sunrise will need
    return (GetMinOfTime(local) < (nightEnd + ((ONE_SUNRISE_STEP_IN_MS * NUM_SUNRISE_STEPS) / 1000  / 60)));
}

/*
   ------------------------------------------------------------------------------
   Configuration parameters configured by the WiFiManager and stored in the FS
   ------------------------------------------------------------------------------
*/

//The default values for the NTP server and the Blynk token, if there are different values in config.json, they are overwritten.
char ntp_server[50] = DEFAULT_NTP;
char blynk_token[33] = "YOUR_BLYNK_TOKEN";

bool IsConfigMode = false;

ClockMode clockMode = eCM_clock;

char station_name[50] = DEFAULT_STATIONNAME;



/* there is no floar/double support in the printf functions, so we need to do that
 * on our own. max up to nine decimals */
char* ftoa(double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};

  char* ret = 0;
  ret = new char[10];
  memset(ret, 0, sizeof(char)*10);

  char* a = ret;
  
  long heiltal = (long) f;
  itoa(heiltal, a, 10);
  
  while (*a != '\0') 
    a++;
  
  *a++ = '.';
  
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  
  return ret;
}



