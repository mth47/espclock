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
CClockDisplay::eDialect clockDialect = CClockDisplay::eD_Ossi;

bool bIsDay = true;

uint16_t GetMinOfTime(int8_t hour, int8_t min)
{
  return (hour * 60) + min;
}

uint16_t GetMinOfTime(time_t t)
{
  return (hour(t) * 60) + minute(t);
}

uint8_t GetHourOfDayMin(uint16_t m)
{
  return m / 60;  
}

uint8_t GetMinOfDayMin(uint16_t m)
{
  return m % 60;  
}

bool IsValidHour(uint8_t h)
{  
  if (0 <= h && 23 >= h)
    return true;

  return false;  
}

bool IsValidDayMin(uint16_t m)
{
  if(IsValidHour(GetHourOfDayMin(m)) && 60 >= GetMinOfDayMin(m))
    return true;

  return false;
}

bool IsNight(time_t local)
{
  static bool bNight=false;
 
  if(nightStart == nightEnd)
  { 
    if(bNight) 
      serialTrace.Log(T_DEBUG, "IsNight: nightStart == nightEnd '%d', return false", nightEnd); 
 
 
    return (bNight = false);
  } 
 
  int16_t currMin = GetMinOfTime(local);

  if (nightEnd > nightStart)
  {
      // the night starts after midnight and and ends in the morning
      if (currMin >= nightStart && currMin < nightEnd)
      {
          if (!bNight)
              serialTrace.Log(T_DEBUG, "IsNight: currMin >= nightStart '%d' && currMin < nightEnd '%d', return true", nightStart, nightEnd);

          return (bNight = true);
      }

      if (bNight)
          serialTrace.Log(T_DEBUG, "IsNight: nightEnd > nightStart '%d', nightEnd '%d', return false", nightStart, nightEnd);

      return (bNight = false);
  }
  else
  {
      // the night starts before midnight and and ends in the morning
      if (currMin >= nightStart || currMin < nightEnd)
      {
          if (!bNight)
              serialTrace.Log(T_DEBUG, "IsNight: currMin >= nightStart '%d' ||  currMin < nightEnd '%d', return true", nightStart, nightEnd);

          return (bNight = true);
      }
  }
 
  if(bNight) 
    serialTrace.Log(T_DEBUG, "IsNight: nightStart > nightEnd '%d', nightEnd '%d', return false", nightStart, nightEnd); 
 
  return (bNight = false);
} 


bool IsNightHour(time_t local)
{
  static bool bNight=false;
  
  if(nightStart == nightEnd)
  {
    if(bNight)
      serialTrace.Log(T_DEBUG, "IsNight: nightStart == nightEnd '%d', return false", nightEnd);

    return (bNight = false);
  }
    
  if(0 == nightStart)
  {
    if(hour(local) >= nightStart && hour(local) < nightEnd)
    {
      if(!bNight)
        serialTrace.Log(T_DEBUG, "IsNight: hour(local) >= nightStart '%d' && hour(local) < nightEnd '%d', return true", nightStart, nightEnd);
      
      return (bNight = true);
    }

    if(bNight)
      serialTrace.Log(T_DEBUG, "IsNight: 0 == nightStart '%d', nightEnd '%d', return false", nightStart, nightEnd);

    return (bNight = false);
  }

  if(0 == nightEnd)
  {
    if(hour(local) >= nightStart && hour(local) > nightEnd)
    {
      if(!bNight)
         serialTrace.Log(T_DEBUG, "IsNight: hour(local) >= nightStart '%d' &&  hour(local) > nightEnd '%d', return true", nightStart, nightEnd);
      
      return (bNight = true);
    }

    serialTrace.Log(T_DEBUG, "IsNight: nightStart '%d', 0 == nightEnd '%d', return false", nightStart, nightEnd);

    return (bNight = false);
  }
  
  if(hour(local) >= nightStart || hour(local) <= nightEnd)
  {
    if(!bNight)
      serialTrace.Log(T_DEBUG, "IsNight: hour(local) >= nightStart '%d' || hour(local) <= nightEnd '%d', return true", nightStart, nightEnd);
    return (bNight = true);
  }

  if(bNight)
    serialTrace.Log(T_DEBUG, "IsNight: nightStart '%d', nightEnd '%d', return false", nightStart, nightEnd);
  
  return (bNight = false);
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
bool IsTestMode = false;

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



