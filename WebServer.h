// based on http://pastebin.com/8uwAhSQ0 by  Gyro Gearloose, 02/2016
// and Stefan Thesen 04/2015 https://blog.thesen.eu/stabiler-http-1-1-wlan-webserver-mit-dem-esp8266-microcontroller/
// plus ideas of Mark Kriegsman and some css references

#include <ESP8266WiFi.h>   // https://links2004.github.io/Arduino/da/de6/class_wi_fi_server.html
#include <FastLED.h>

#include "CClockDisplay.h"

#define TABLE_BORDER_COLOR "#e7e7de"  // same gray like the button background
#define TABLE_WIDTH "500px"

class WebServer
{
public:

  WebServer();

  virtual ~WebServer();

  // blocks until connected to WLAN
  void Configure();
  
  class wsRequest
  {
    public:
    
    wsRequest(CRGB color, uint8_t brightnessNight, uint8_t brightnessDay, uint8_t brightness, String ntp, uint8_t nightStart, uint8_t nightEnd, CClockDisplay::eDialect dia)
    {
      m_color = color;
      m_brightnessNight = brightnessNight;
      m_brightnessDay = brightnessDay;
      m_brightness = brightness;
      m_bEnableConfigMode = false;
      m_bToggleTestMode = false;
      m_bColorChanged = false;
      m_bBrightnessChanged = false;
      m_ntp = ntp;
      m_bNtpChanged = false;
      m_dia = dia;
      m_bDialectChanged = false;
      m_nightStart = nightStart;
      m_nightEnd = nightEnd;
      m_bNightChanged = false;
    }

    virtual ~wsRequest() {}

    bool HasColorChanged () { return m_bColorChanged; }
    bool HasBrightnessChanged () { return m_bBrightnessChanged; }   
    bool HasNtpChanged () { return m_bNtpChanged; }
    bool HasNightChanged () { return m_bNightChanged; }
    bool HasDialectChanged () { return m_bDialectChanged; }
    
    void SetDialect(CClockDisplay::eDialect dia) 
    { 
      if(dia != m_dia)
        m_bDialectChanged = true;  
        
      m_dia = dia;
    }
    CClockDisplay::eDialect GetDialect() { return m_dia; }
     
    void SetColor(CRGB color) 
    { 
      if(m_color.r != color.r)
        m_bColorChanged = true;

      if(m_color.g != color.g)
        m_bColorChanged = true;

      if(m_color.b != color.b)
        m_bColorChanged = true;
      
      m_color = color; 
    }
    CRGB GetColor() { return m_color; }
    
    void SetColorRed(uint8_t r) 
    { 
      if(m_color.r != r)
        m_bColorChanged = true;
    
      m_color.r = r;
    }

    void SetColorGreen(uint8_t g) 
    { 
      if(m_color.g != g)
        m_bColorChanged = true;
    
      m_color.g = g;
    }

    void SetColorBlue(uint8_t b) 
    { 
      if(m_color.b != b)
        m_bColorChanged = true;
    
      m_color.b = b;
    }

    void SetBrightness(uint8_t b) 
    { 
      if(m_brightness != b)
        m_bBrightnessChanged = true;
    
      m_brightness = b;
    }
    uint8_t GetBrightness() { return m_brightness; }

    void SetBrightnessDay(uint8_t b) 
    { 
      if(m_brightnessDay != b)
        m_bBrightnessChanged = true;
    
      m_brightnessDay = b;
    }
    uint8_t GetBrightnessDay() { return m_brightnessDay; }

    void SetBrightnessNight(uint8_t b) 
    { 
      if(m_brightnessNight != b)
        m_bBrightnessChanged = true;
    
      m_brightnessNight = b;
    }
    uint8_t GetBrightnessNight() { return m_brightnessNight; }
    
    void SetNightStart(uint8_t b) 
    { 
      if(m_nightStart != b)
        m_bNightChanged = true;
    
      m_nightStart = b;
    }
    uint8_t GetNightStart() { return m_nightStart; }

    void SetNightEnd(uint8_t b) 
    { 
      if(m_nightEnd != b)
        m_bNightChanged = true;
    
      m_nightEnd = b;
    }
    uint8_t GetNightEnd() { return m_nightEnd; }

    void SetNtp(String ntp) 
    { 
      if(m_ntp != ntp)
        m_bNtpChanged = true;
    
      m_ntp = ntp;
    }
    String GetNtp() { return m_ntp; }

    bool m_bToggleTestMode;
    bool m_bEnableConfigMode;
    
    private:

    bool m_bColorChanged;
    bool m_bBrightnessChanged;
    
    CRGB m_color;
    uint8_t m_brightnessNight;
    uint8_t m_brightnessDay;
    uint8_t m_brightness;  
    String m_ntp;
    bool m_bNtpChanged;
    uint8_t m_nightStart;
    uint8_t m_nightEnd;
    bool m_bNightChanged;

    CClockDisplay::eDialect m_dia;
    bool m_bDialectChanged;
    
  };

  typedef void (wsRequest::*SetIntFn) (uint8_t);
  typedef void (wsRequest::*SetStringFn) (String);
  
  void Run(wsRequest& request);
  
private:

  String RGBToHexString(CRGB rgb);  
  String ColorValueToHex(int8_t color);
  int8_t HexColorValueToInt(String hc);
  /*void AddColorPickerScript(String &msg, String htmlColor, String Id);
  void AddColorPicker(String &msg, String Id);*/
  void AddSliderControl(String &msg, String htmlColor, String label, String rangeName, String rangeId, int16_t minValue, int16_t maxValue, int16_t curColor, String inputName, String inputId);
  void AddTextField(String &msg, String htmlColor, String label, String fieldName, String fieldId, String fieldValue, bool bTable = true);
  void AddNumberField(String &msg, String label, String fieldName, String fieldId, float fieldValue, bool bTable = true, bool bEdit = true);
  void AddButton(String &msg, String label, String fieldName, String buttonValue);
  void AddCheckBox(String &msg, String label, String fieldName, String fieldId, String value, String currentValue, String currentValueColor);
  void AddRadioButton(String &msg, String label, String fieldName, String fieldId, String value, bool bChecked);
  void ReadSliderValue(String& sCmd, const char* paramLabel, wsRequest& request, SetIntFn fn);
  // ColorChoice=Slider ( or Picker)
  bool UseSlider(String& sCmd, const char* paramLabel);
  void ReadTextField(String& sCmd, const char* paramLabel, wsRequest& request, SetStringFn fn);
  void ReadNumberField(String& sCmd, const char* paramLabel, wsRequest& request, SetIntFn fn);
  //colorPicker=%237c3400 == #7c3400
  void ReadColorPicker(String& sCmd, const char* paramLabel, wsRequest& request);
  String ReadValueAsString(String& sCmd, const char* paramLabel);
  void SkipValue(String& sCmd);
  
  unsigned long ulReqcount;
  unsigned long ulReconncount;

  WiFiServer server = WiFiServer(80);  // Create an instance of the server on Port 80

  const char* htmlStyleCSS =   "<style type=\"text/css\">"
                                "input[type=range] {"
                                "/*removes default webkit styles*/"
                                "-webkit-appearance: none;"
                                "/*fix for FF unable to apply focus style bug */"
                                "border: 1px solid white;"
                                "/*required for proper track sizing in FF*/"
                                "width: 300px;"
                                "}"
                                "input[type=range]::-webkit-slider-runnable-track {"
                                "  width: 300px;"
                                "  height: 5px;"
                                "  background: #ddd;"
                                "  border: none;"
                                "  border-radius: 3px;"
                                "}"
                                "input[type=range]::-webkit-slider-thumb {"
                                "  -webkit-appearance: none;"
                                "  border: none;"
                                "  height: 16px;"
                                "  width: 16px;"
                                "  border-radius: 50%;"
                                "  background: goldenrod;"
                                "  margin-top: -4px;"
                                "}"
                                "input[type=range]:focus {"
                                "  outline: none;"
                                "}"
                                "input[type=range]:focus::-webkit-slider-runnable-track {"
                                "  background: #ccc;"
                                "}"
                                "input[type=range]::-moz-range-track {"
                                "  width: 300px;"
                                "  height: 5px;"
                                "  background: #ddd;"
                                "  border: none;"
                                "  border-radius: 3px;"
                                "}"
                                "input[type=range]::-moz-range-thumb {"
                                "  border: none;"
                                "  height: 16px;"
                                "  width: 16px;"
                                "  border-radius: 50%;"
                                "  background: goldenrod;"
                                "}"
                                "/*hide the outline behind the border*/"
                                "input[type=range]:-moz-focusring{"
                                "  outline: 1px solid white;"
                                "  outline-offset: -1px;"
                                "}"
                                "input[type=range]::-ms-track {"
                                "  width: 300px;"
                                "  height: 5px;"
                                "  /*remove bg colour from the track, we'll use ms-fill-lower and ms-fill-upper instead */"
                                "  background: transparent;  "
                                "  /*leave room for the larger thumb to overflow with a transparent border */"
                                "  border-color: transparent;"
                                "  border-width: 6px 0;"
                                "  /*remove default tick marks*/"
                                "  color: transparent;"
                                "}"
                                "input[type=range]::-ms-fill-lower {"
                                "  background: #777;"
                                "  border-radius: 10px;"
                                "}"
                                "input[type=range]::-ms-fill-upper {"
                                "  background: #ddd;"
                                "  border-radius: 10px;"
                                "}"
                                "input[type=range]::-ms-thumb {"
                                "  border: none;"
                                "  height: 16px;"
                                "  width: 16px;"
                                "  border-radius: 50%;"
                                "  background: goldenrod;"
                                "}"
                                "input[type=range]:focus::-ms-fill-lower {"
                                "  background: #888;"
                                "}"
                                "input[type=range]:focus::-ms-fill-upper {"
                                "  background: #ccc;"
                                "}"
                                ".button:hover {"
                                "  -webkit-transition-duration: 0.4s; /* Safari */"
                                "  transition-duration: 0.4s;"
                                "  box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);"
                                "  background-color:  #d7d7d7; /* Gray */"
                                "}"
                                ".button {"
                                "  border: none;"
                                "  padding: 14px 40px;"
                                "  border-radius: 12px;"
                                "  width: 100%;"
                                "  background-color: #e7e7e7; color: black;"
                                "}"
                                ".button1 { width: 100%;}"
                                "input[type=submit]:hover {"
                                "  -webkit-transition-duration: 0.4s; /* Safari */"
                                "  transition-duration: 0.4s;"
                                "  box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);"
                                "  background-color:  ; /* Gray */"
                                "}"
                                "input[type=submit] {"
                                "  border: none;"
                                "  padding: 14px 40px;"
                                "  border-radius: 12px;"
                                "  width: 100%;"
                                "  background-color: #e7e7e7; color: black;"
                                "}"
                                "input[type=text] {"
                                "  border-radius: 4px;"
                                "}"
                                "input[type=number] {"
                                "  border-radius: 4px;"
                                "}"
                                /*"input[type=email] {"
                                "  border-radius: 4px;"
                                "}"*/
                                "table { "
                                "  border: 1px solid " TABLE_BORDER_COLOR ";"
                                "  table-layout: fixed;"
                                "}"                                
                                "</style>";


   const char* autoRefreshScript = "<script type=\"text/javascript\">"
                                   "if(window.location.href.substr(-2) !== \"?r\") {"
                                      "setTimeout(function() {window.location = window.location.href = \"\?r\"}, 1000);"
                                   "}"
                                   "</script>";        
};  


