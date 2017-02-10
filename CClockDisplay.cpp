#include "CClockDisplay.h"
#include "GlobalsAndDefines.h"

/*
   Based on https://github.com/thomsmits/wordclock

   Copyright (c) 2012 Thomas Smits

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
   documentation files (the "Software"), to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions
   of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
   TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
   
   
   Note: Several modifications by mth47.

*/

/**
  Positions of the text on the LED panel.

  As the Arduino programming language cannot determine
  the length of an array that is passed to a function
  as a pointer, a -1 at the end of the array indicates the
  end of the array.

  The values of the array have to be <= NUMBER_OF_LEDS - 1.
*/
const int ES[]      = { 0, 1, -1 };
const int IST[]     = { 3, 4, 5, -1 };
const int FUENF_M[] = { 7, 8, 9, 10, -1 };
const int ZEHN_M[]  = { 18, 19, 20, 21, -1 };
const int ZWANZIG[] = { 11, 12, 13, 14, 15, 16, 17, -1 };
const int DREIVIERTEL[]  = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1 };
const int VIERTEL[] = { 26, 27, 28, 29, 30, 31, 32, -1 };
const int VOR[]     = { 35, 36, 37, -1 };
const int FUNK[]    = { 36, 37, 38, 39, -1 };
const int NACH[]    = { 38, 39, 40, 41, -1 };
const int HALB[]    = { 44, 45, 46, 47, -1 };
const int ELF[]     = { 85, 86, 87, -1 };
const int FUENF[]   = { 73, 74, 75, 76, -1 };
const int EIN[]     = { 61, 62, 63, -1 };
const int EINS[]    = { 60, 61, 62, 63, -1 };
const int ZWEI[]    = { 62, 63, 64, 65, -1 };
const int DREI[]    = { 67, 68, 69, 70, -1 };
const int VIER[]    = { 77, 78, 79, 80, -1 };
const int SECHS[]   = { 104, 105, 106, 107, 108, -1 };
const int ACHT[]    = { 89, 90, 91, 92, -1 };
const int SIEBEN[]  = { 55, 56, 57, 58, 59, 60, -1 };
const int ZWOELF[]  = { 49, 50, 51, 52, 53, -1 };
const int ZEHN[]    = { 93, 94, 95, 96, -1 };
const int UHR[]     = { 99, 100, 101, -1 };
const int NEUN[]    = { 81, 82, 83, 84, -1 };
// for small clock
const int MIN1_S[]    = { 112, -1 };
const int MIN2_S[]    = { 112, 113, -1 };
const int MIN3_S[]    = { 112, 113, 114, -1 };
const int MIN4_S[]    = { 112, 113, 114, 115, -1 };
// for big clock
const int MIN1[]    = { 121, -1 };
const int MIN2[]    = { 121, 120, -1 };
const int MIN3[]    = { 121, 120, 110, -1 };
const int MIN4[]    = { 121, 120, 110, 119, -1 };

const int DST[]     = { 109, -1};

const char* STR_ES_IST_  = "Es ist ";
const char* STR_FUENFH_  = "Fünf ";
const char* STR_ZEHNH_   = "Zehn ";
const char* STR_FUENF_   = "fünf ";
const char* STR_ZEHN_    = "zehn ";
const char* STR_ZWANZIG_ = "zwanzig ";
const char* STR_DREIVIERTEL_ = "dreiviertel ";
const char* STR_VIERTEL_ = "viertel ";
const char* STR_VOR_     = "vor ";
const char* STR_NACH_    = "nach ";
const char* STR_HALB_    = "halb ";
const char* STR_ELF_     = "Elf ";
const char* STR_EIN_     = "Ein ";
const char* STR_EINS_    = "Eins ";
const char* STR_ZWEIH_   = "Zwei ";
const char* STR_DREIH_   = "Drei ";
const char* STR_VIERH_   = "Vier ";
const char* STR_ZWEI_    = "zwei ";
const char* STR_DREI_    = "drei ";
const char* STR_VIER_    = "vier ";
const char* STR_SECHS_   = "Sechs ";
const char* STR_ACHT_    = "Acht ";
const char* STR_SIEBEN_  = "Sieben ";
const char* STR_ZWOELF_  = "Zwölf ";
const char* STR_UHR_     = "Uhr ";
const char* STR_NEUN_    = "Neun ";
const char* STR_UND_     = "und ";
const char* STR_MINUTEN  = "Minuten";
const char* STR_EINE_MINUTE  = "eine Minute";


// class CClockDisplay::ClockString
CClockDisplay::ClockString::ClockString(int numLEDs) : m_cs(0), m_csLen(0)
{
    // every leds represents one charachter, so this is the maximum clockString size as well
	// additional we add the number of minutes not part of the main display
	m_csLen = strlen("plus x Minuten") + numLEDs + 1;
	m_cs = new char[m_csLen];
	if(m_cs)
	{
		Clear();
	}	
}

CClockDisplay::ClockString::~ClockString()
{	
	delete [] m_cs;
	m_cs = 0;
}

void CClockDisplay::ClockString::Clear() 
{ 
	memset(m_cs, 0, m_csLen); 
}

// Result in "Es ist "
void CClockDisplay::ClockString::Reset() 
{ 
    Clear();
	strcpy(m_cs, STR_ES_IST_);
}

void CClockDisplay::ClockString::Add(const char* s)
{
	strcat(m_cs, s);
}

const char* CClockDisplay::GetHourString(int h, int m)
{
    if(0 == h || 12 == h || 24 == h)
	  return STR_ZWOELF_;
	
    if(11 == h || 23 == h)
	  return STR_ELF_;	
	
    if(10 == h || 22 == h)
	  return STR_ZEHNH_;	
	  
    if(9 == h || 21 == h)
	  return STR_NEUN_;	
	  
    if(8 == h || 20 == h)
	  return STR_ACHT_;		  
	
    if(7 == h || 19 == h)
	  return STR_SIEBEN_;	
	  
    if(6 == h || 18 == h)
	  return STR_SECHS_;	
	  
    if(5 == h || 17 == h)
	  return STR_FUENFH_;	
	  
    if(4 == h || 16 == h)
	  return STR_VIERH_;	
	  
    if(3 == h || 15 == h)
	  return STR_DREIH_;	
	  
    if(2 == h || 14 == h)
	  return STR_ZWEIH_;	
	  
    if((1 == h || 13 == h) && 0 != m)
	  return STR_EINS_;

	if((1 == h || 13 == h) && 0 == m)
	  return STR_EIN_;
	  
	return "";
}

// class CClockDisplay
CClockDisplay::CClockDisplay() : m_pLEDs(0), m_numLEDs(0), m_color(CRGB::Red), m_currentMinute(-1), m_pTZ(0), m_bSmallClock(false), m_bDialect(eD_Ossi), m_clockString(0)
{
	m_clockString = new ClockString(m_numLEDs);	
}

CClockDisplay::~CClockDisplay()
{
   delete m_clockString;
   m_clockString = 0;
}

bool CClockDisplay::setup(CRGB* leds, int numLEDs)
{
  m_pLEDs = leds;
  m_numLEDs = numLEDs;

  serialTrace.Log(T_DEBUG, "CClockDisplay::setup - initialized for BIG CLOCK");
  serialTrace.Log(T_DEBUG, "CClockDisplay::setup - Number of LEDs = %d", m_numLEDs);

  return true;
}

bool CClockDisplay::setup_small(CRGB* leds, int numLEDs)
{
  m_pLEDs = leds;
  m_numLEDs = numLEDs;

  serialTrace.Log(T_DEBUG, "CClockDisplay::setup - initialized for SMALL CLOCK");
  serialTrace.Log(T_DEBUG, "CClockDisplay::setup - Number of LEDs = %d", m_numLEDs);

  m_bSmallClock = true;

  return true;
}

const char* CClockDisplay::GetClockString ()
{
	if(!m_clockString)
		return "";
		
	return m_clockString->Get();
}

bool CClockDisplay::update(bool force)
{
  if (m_currentMinute != minute() || true == force)
  {
    time_t utc(now());
    fill_solid( &(m_pLEDs[0]), m_numLEDs, CRGB::Black);

    compose(ES);
    compose(IST);

    //  if(CE.utcIsDST(utc))
    //  {
    //    compose(DST);
    //  }

    time_t local(0 == m_pTZ ? utc : m_pTZ->toLocal(utc));
    display_time(hour(local), minute(local));

    m_currentMinute = minute();
    Serial.println();
    return true;
  }

  return false;
}

CRGB CClockDisplay::getColor()
{
  return (m_color);
}

void CClockDisplay::setColor(const CRGB& color)
{
  m_color = color;
}

void CClockDisplay::setTimezone(Timezone* pTZ)
{
  m_pTZ = pTZ;
}

void CClockDisplay::SetDialect(eDialect dia)
{
  m_bDialect = dia;
}

CClockDisplay::eDialect CClockDisplay::GetDialect()
{
  return m_bDialect;
}

bool CClockDisplay::IsWessiDialect ()
{
  return (eD_Wessi == m_bDialect) ? true : false;
}

bool CClockDisplay::IsOssiDialect ()
{
  return (eD_Ossi == m_bDialect) ? true : false;
}

bool CClockDisplay::IsRheinRuhrDialect ()
{
  return (eD_RheinRuhr == m_bDialect) ? true : false;
}

/**
  Sets the bits in resultArray depending on the values of the
  arrayToAdd. Bits already set in the input array will not
  be set to zero.

  WARNING: this function does not perform any bounds checks!

  @param arrayToAdd Array containing the bits to be set as
        index of the bit. The value 5 for example implies
        that the 6th bit in the resultArray has to be set to 1.
        This array has to be terminated by -1.

  @param (out) ledBits Array where the bits will be set according
        to the input array. The ledBits has to be big enough to
        accomodate the indices provided in arrayToAdd.
*/
void CClockDisplay::compose(const int arrayToAdd[]) {
  int pos;
  int i = 0;

  while ((pos = arrayToAdd[i++]) != -1) {
    m_pLEDs[pos] = m_color;
  }
}

/**
  Sets the hour information for the clock.

  @param hour the hour to be set
  @param (out) ledBits array to set led bits
*/
void CClockDisplay::display_hour(const int displayHour, const int minute, const int hour) {

  int hourAMPM = displayHour;

  //  if (hour >= 12) {
  //    compose(PM, ledBits);
  //  }

  if (displayHour >= 12) {
    hourAMPM -= 12;
  }

  Serial.print("display_hour: ");
  Serial.print("hour=");
  Serial.print(hour);
  Serial.print(", hourAMPM=");
  Serial.print(hourAMPM);


  switch (hourAMPM) {

    case 0: compose(ZWOELF);
      break;

    case 1:
      if (minute == 0) {
        compose(EIN);
      }
      else {
        compose(EINS);
      }
      break;

    case 2: compose(ZWEI);
      break;

    case 3: compose(DREI);
      break;

    case 4: compose(VIER);
      break;

    case 5: compose(FUENF);
      break;

    case 6: compose(SECHS);
      break;

    case 7: compose(SIEBEN);
      break;

    case 8: compose(ACHT);
      break;

    case 9: compose(NEUN);
      break;

    case 10: compose(ZEHN);
      break;

    case 11: compose(ELF);
      break;

    case 12: compose(ZWOELF);
      break;
  }
}

/**
  Displays hour and minutes on the LED panel.

  @param hour the hour to be set
  @param minute the minute to be set
  @param (out) ledBits bits for the LEDs (will NOT be cleared)
*/
void CClockDisplay::display_time(const int hour, const int minute) {

  int roundMinute = (minute / 5) * 5;
  int minutesRemaining = minute - roundMinute;

  Serial.print("display_time: ");
  Serial.print("roundMinute=");
  Serial.print(roundMinute);
  Serial.print(" minutesRemaining=");
  Serial.print(minutesRemaining);

  int displayHour = hour;
  
  m_clockString->Reset();

  switch (roundMinute) {
    case 0:
      compose(UHR);
      Serial.print(", case 0");
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
	  m_clockString->Add(STR_UHR_);
      break;

    case 5:
      compose(FUENF_M);
      compose(NACH);
      Serial.print(", case 5 ");
	  m_clockString->Add(STR_FUENF_);
	  m_clockString->Add(STR_NACH_);
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 10:
      compose(ZEHN_M);
      compose(NACH);
      Serial.print(", case 10");
	  m_clockString->Add(STR_ZEHN_);
	  m_clockString->Add(STR_NACH_);
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 15:
      if (IsOssiDialect())
      {
        compose(VIERTEL);
        displayHour++;
        Serial.print(", case 15-o");
		m_clockString->Add(STR_VIERTEL_);
      }
      else
      {
        compose(VIERTEL);
        compose(NACH);
        Serial.print(", case 15-!o");
		m_clockString->Add(STR_VIERTEL_);
		m_clockString->Add(STR_NACH_);
      }
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 20:
      if (IsWessiDialect() || IsOssiDialect())
      {
        compose(ZEHN_M);
        compose(VOR);
        compose(HALB);
        displayHour++;
        Serial.print(", case 20-ow");
		m_clockString->Add(STR_ZEHN_);
	    m_clockString->Add(STR_VOR_);
		m_clockString->Add(STR_HALB_);
      }
      else
      {
        compose(ZWANZIG);
        compose(NACH);
        Serial.print(", case 20-rr");
		m_clockString->Add(STR_ZWANZIG_);
	    m_clockString->Add(STR_NACH_);
      }
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 25:
      compose(FUENF_M);
      compose(VOR);
      compose(HALB);
      displayHour++;
      Serial.print(", case 25");
	  m_clockString->Add(STR_FUENF_);
	  m_clockString->Add(STR_VOR_);
	  m_clockString->Add(STR_HALB_);
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 30:
      compose(HALB);
      displayHour++;
      Serial.print(", case 30");
	  m_clockString->Add(STR_HALB_);
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 35:
      compose(FUENF_M);
      compose(NACH);
      compose(HALB);
      displayHour++;
      Serial.print(", case 35");
	  m_clockString->Add(STR_FUENF_);
	  m_clockString->Add(STR_NACH_);
	  m_clockString->Add(STR_HALB_);
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 40:
      if (IsWessiDialect() || IsOssiDialect())
      {
        compose(ZEHN_M);
        compose(NACH);
        compose(HALB);
        displayHour++;
        Serial.print(", case 40-ow");
		m_clockString->Add(STR_ZEHN_);
	    m_clockString->Add(STR_NACH_);
	    m_clockString->Add(STR_HALB_);
      }
      else
      {
        compose(ZWANZIG);
        compose(VOR);
        displayHour++;
        Serial.print(", case 40-rr");
		m_clockString->Add(STR_ZWANZIG_);
	    m_clockString->Add(STR_VOR_);
      }
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 45:
      if (IsOssiDialect())
      {
        compose(DREIVIERTEL);
        displayHour++;
        Serial.print(", case 45-o");
		m_clockString->Add(STR_DREIVIERTEL_);
      }
      else
      {
        compose(VIERTEL);
        compose(VOR);
        displayHour++;
        Serial.print(", case 45-wrr");
		m_clockString->Add(STR_VIERTEL_);
		m_clockString->Add(STR_VOR_);
      }
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 50:
      compose(ZEHN_M);
      compose(VOR);
      displayHour++;
      Serial.print(", case 50");
	  m_clockString->Add(STR_ZEHN_);
	  m_clockString->Add(STR_VOR_);
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    case 55:
      compose(FUENF_M);
      compose(VOR);
      displayHour++;
      Serial.print(", case 55");
	  m_clockString->Add(STR_FUENF_);
	  m_clockString->Add(STR_VOR_);
	  m_clockString->Add(GetHourString(displayHour, roundMinute));
      break;

    default:
      Serial.print(", default-case");
  }
  
  if (m_bSmallClock)
  {
    switch (minutesRemaining)
    {

      case 1: compose(MIN1_S);
        Serial.print(", case MIN1_S ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_EINE_MINUTE);
        break;
      case 2: compose(MIN2_S);
        Serial.print(", case MIN2_S ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_ZWEI_);
		m_clockString->Add(STR_MINUTEN);
        break;
      case 3: compose(MIN3_S);
        Serial.print(", case MIN3_S ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_DREI_);
		m_clockString->Add(STR_MINUTEN);
        break;
      case 4: compose(MIN4_S);
        Serial.print(", case MIN4_S ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_VIER_);
		m_clockString->Add(STR_MINUTEN);
        break;
      default: 
        Serial.print(", ");
      break;
    }
  }
  else
  {
    switch (minutesRemaining)
    {
      case 1: compose(MIN1);
        Serial.print(", case MIN1 ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_EINE_MINUTE);
        break;
      case 2: compose(MIN2);
        Serial.print(", case MIN2 ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_ZWEI_);
		m_clockString->Add(STR_MINUTEN);
        break;
      case 3: compose(MIN3);
        Serial.print(", case MIN3 ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_DREI_);
		m_clockString->Add(STR_MINUTEN);
        break;
      case 4: compose(MIN4);
        Serial.print(", case MIN4 ");
        m_clockString->Add(STR_UND_);
		m_clockString->Add(STR_VIER_);
		m_clockString->Add(STR_MINUTEN);
        break;
      default: break;
    }
  }

  display_hour(displayHour, roundMinute, hour);

}

