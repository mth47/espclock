#include "GlobalsAndDefines.h"

#include <FastLED.h>
#include "CFadeAnimation.h"

#include <ESP8266WiFi.h>

#include <DNSServer.h>            
#include <ESP8266WebServer.h>     
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#include <Ticker.h>
Ticker ticker;

#ifdef ENABLE_BLYNK

  #ifdef TRACE
    #ifdef _DEBUG
      #define BLYNK_PRINT Serial        // Comment this out to disable prints and save space
    #endif
  #endif

  #include <BlynkSimpleEsp8266.h>
  #include <SimpleTimer.h>
  
  #define BLYNK_CONNECT_TIMEOUT_SEC 10                 // mthomas: waits max 10 seconds for blink connection
#endif

/*
 * ------------------------------------------------------------------------------
 * Start Configuration Section
 * ------------------------------------------------------------------------------
 */
 

/* 
 * ------------------------------------------------------------------------------
 * Open Pixel Control Server configuration
 * ------------------------------------------------------------------------------
*/
#ifdef ENABLE_OPC

#include "OpcServer.h"            //https://github.com/plasticrake/OpcServer (use a version which includes the following commit: https://github.com/plasticrake/OpcServer/commit/32eafd7d13799ca23b570ec2547ff225f01c015d)

const int OPC_PORT = 7890;
const int OPC_CHANNEL = 1;      // Channel to respond to in addition to 0 (broadcast channel)
const int OPC_MAX_CLIENTS = 2;  // Maxiumum Number of Simultaneous OPC Clients
const int OPC_MAX_PIXELS = NUM_LEDS;
const int OPC_BUFFER_SIZE = OPC_MAX_PIXELS * 3 + OPC_HEADER_BYTES;
#endif

/*
 * ------------------------------------------------------------------------------
 * Clock configuration/variables
 * ------------------------------------------------------------------------------
 */

bool displayClock = true;

/*
 * ------------------------------------------------------------------------------
 * End Configuration Section
 * ------------------------------------------------------------------------------
*/

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

bool IsValidHour(uint8_t h)
{  
  if (0 <= h && 23 >= h)
    return true;

  return false;  
}

//------------------------------------------------------------------------------
// permanent storage

void ReadTheConfig(CRGB &ledcolor, CClockDisplay::eDialect& dia)
{
  if (SPIFFS.exists("/config.json")) 
  {
    //file exists, reading and loading
    serialTrace.Log(T_INFO, "reading config file");
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) 
    {
      serialTrace.Log(T_INFO, "opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);

      configFile.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      json.printTo(Serial);
      Serial.println("");
      if (json.success()) 
      {
        if(0 != json["ntp_server"].as<const char*>())
          strcpy(ntp_server, json["ntp_server"]);

        if(0 != json["blynk_token"].as<const char*>())
          strcpy(blynk_token, json["blynk_token"]);

        ledcolor.r = json["red"];
        if(ledcolor.r > 255 || ledcolor.r < 0)
          ledcolor.r = DEFAULT_RED;
        ledcolor.g= json["green"];
        if(ledcolor.g > 255 || ledcolor.g < 0)
          ledcolor.g = DEFAULT_GREEN;
        ledcolor.b = json["blue"];
        if(ledcolor.b > 255 || ledcolor.b < 0)
          ledcolor.b = DEFAULT_BLUE;
        brightnessNight = (uint8_t) json["brightnessNight"];
        if(brightnessNight > 1024 || brightnessNight < 0) // it should be possible to switch off the display in the night hours
          brightnessNight = (uint8_t) 5;
        brightnessDay = (uint8_t) json["brightnessDay"];
        if(brightnessDay > 1024 || brightnessDay <= 0)
          brightnessDay = (uint8_t) 50;
        brightness = (uint8_t) json["brightness"];
        if(brightness > 1024 || brightness < 0)
          brightness = (uint8_t) brightnessDay;
        int tmp = json["IsConfigMode"];
        (1 == tmp) ? IsConfigMode = true : IsConfigMode = false;

        // make sure a visible color is chosen
        if(0 == ledcolor.r && 0 == ledcolor.g && 0 == ledcolor.b)
          ledcolor.r = 255;

        tmp = json["Dialect"];
        dia = (CClockDisplay::eDialect) tmp;
        if(CClockDisplay::eD_Wessi != dia && CClockDisplay::eD_RheinRuhr != dia && CClockDisplay::eD_Ossi != dia)
          dia == CClockDisplay::eD_Ossi;

        nightStart = (uint8_t) json["nightStart"];
        if(!IsValidHour(nightStart))
          nightStart = (uint8_t) 22;

        nightEnd = (uint8_t) json["nightEnd"];
        if(!IsValidHour(nightEnd))
          nightEnd = (uint8_t) 6;

        if(0 != json["station_name"].as<const char*>())
          strcpy(station_name, json["station_name"]);          

        serialTrace.Log(T_INFO, "parsed json");
          
      } 
      else 
      {
        serialTrace.Log(T_ERROR, "failed to load json config");
      }
    }
  }
}

void SaveTheConfig(CRGB ledcolor)
{
  serialTrace.Log(T_INFO, "saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  json["ntp_server"] = ntp_server;
  json["blynk_token"] = blynk_token;

  // looks like only int is supported or at least size of int is reqad even in case
  // smaller numeric types are used????
  int tmp = -1;
  
  // save the current led settings as well
  tmp = ledcolor.r; json["red"] = tmp;
  tmp = ledcolor.g; json["green"] = tmp;
  tmp = ledcolor.b; json["blue"] = tmp;
  tmp = brightnessNight; json["brightnessNight"] = tmp;
  tmp = brightnessDay; json["brightnessDay"] = tmp;
  tmp = brightness; json["brightness"] = tmp;
  
  if(IsConfigMode)
    json["IsConfigMode"] = 1;
  else
    json["IsConfigMode"] = 0;
  
  tmp = clock.GetDialect(); json["Dialect"] = tmp;
  tmp = nightStart; json["nightStart"] = tmp;
  tmp = nightEnd; json["nightEnd"] = tmp;
  
  json["station_name"] = station_name;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    serialTrace.Log(T_ERROR, "failed to open config file for writing");
  }

  serialTrace.Log(T_INFO, "SaveTheConfig - New settings:");
  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
  Serial.println("");
  
  //end save
}
//------------------------------------------------------------------------------


CNTPClient Ntp;

//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);

CFadeAnimation ani;

bool IsNight(time_t local)
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

void updateBrightness(bool force=false)
{
  static bool bDay=true;
  bool bConfigChanged = false; // we should not change write the config, if nothing changed

  time_t local(CE.toLocal(now()));  
  
  if(IsNight(local))
  {
    if(true == bDay || true == force)
    {
      Serial.println("Updating the brightness for the night.");
      if(brightness != brightnessNight)
        bConfigChanged = true;

      brightness = brightnessNight;
      
#ifdef ENABLE_BLYNK
      if (true == Blynk.connected())
        Blynk.virtualWrite(V1, brightness);
#endif
      FastLED.setBrightness(brightness);
      bDay = false;  
    }
  }
  else if (false == bDay || true == force)
  {
    Serial.println("Updating the brightness for the day.");
    if(brightness != brightnessDay)
      bConfigChanged = true;

    brightness = brightnessDay;
        
#ifdef ENABLE_BLYNK
    if (true == Blynk.connected())
      Blynk.virtualWrite(V1, brightness);
#endif
    FastLED.setBrightness(brightness);
    bDay = true;
  }

  if(bConfigChanged)
    SaveTheConfig(clock.getColor());
}


void updateClock(CRGB ledcolor)
{
    clock.setColor(ledcolor);
    clock.update(true);
    // save the color back to our internal storage
    SaveTheConfig(ledcolor);
}

/*
 * ------------------------------------------------------------------------------
 * Callbacks
 * ------------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
//Time callback:

time_t getDateTimeFromRTC()
{
  return Rtc.now();
}

#ifdef ENABLE_BLYNK

SimpleTimer BlynkConnTimer;

//------------------------------------------------------------------------------
//Blynk callbacks:

void reconnectBlynk()
{
  static bool trBlynkConnState = false;

  if (true == Blynk.connected())
  {  
      if(false == trBlynkConnState)
      {
          serialTrace.Log(T_INFO, "reconnectBlynk - Blynk connected.");  
          trBlynkConnState = true;
      }
  }
  else
  {
      if(true == trBlynkConnState)
      {
          serialTrace.Log(T_ERROR, "reconnectBlynk - Blynk not connected. Trying to reconnect ... ");
          trBlynkConnState = false;
          // clean-up needed
          // Blynk.disconnect();
      }
      else // give the server and WLAN another timer duration before we reconnect the first time.
      {
        serialTrace.Log(T_DEBUG, "reconnectBlynk - Blynk not connected. Calling explicit connect ... ");
        // Blynk.config(blynk_token);
        Blynk.connect(); // Blynk.connect is in 3ms units. Trying it one time.
      }
  }
}

BLYNK_WRITE(V0)
{
  if (true == Blynk.connected())
  {
    // serialTrace.Log(T_DEBUG, "BLYNK_WRITE(V0), param = %s", param);
    CRGB ledcolor;
    ledcolor.r = param[0].asInt();
    ledcolor.g = param[1].asInt();
    ledcolor.b = param[2].asInt();

    updateClock(ledcolor);
  }
}

BLYNK_READ(V0)
{
  CRGB ledcolor(clock.getColor());

  const char* param[3];
  String r = String(ledcolor.r);
  String g = String(ledcolor.g);
  String b = String(ledcolor.b);
  
  param[0] = r.c_str();
  param[1] = g.c_str();
  param[2] = b.c_str();
  
  Blynk.virtualWrite(V0, ledcolor.r, ledcolor.g, ledcolor.b);
}

BLYNK_READ(V1)
{
  if (true == Blynk.connected())
    Blynk.virtualWrite(V1, brightness);
}

BLYNK_WRITE(V1)
{
  if (true == Blynk.connected())
  {
    brightness = param.asInt();
    FastLED.setBrightness(brightness);
  }
}

BLYNK_READ(V2)
{
  if (true == Blynk.connected())
    Blynk.virtualWrite(V2, brightnessDay);
}

BLYNK_WRITE(V2)
{
  if (true == Blynk.connected())
  {
    brightnessDay = param.asInt();
    updateBrightness(true);
  }
}

BLYNK_READ(V3)
{
  if (true == Blynk.connected())
    Blynk.virtualWrite(V3, brightnessNight);
}

BLYNK_WRITE(V3)
{
  if (true == Blynk.connected())
  {
    brightnessNight = param.asInt();
    updateBrightness(true);
  }
}

BLYNK_CONNECTED() 
{
  static bool isFirstConnect = true;
  if (isFirstConnect) 
  {
    // Request Blynk server to re-send latest values
    Blynk.syncAll();
    isFirstConnect = false;
  }
    
  updateBrightness(true);
  Blynk.virtualWrite(V1, brightness);
}
#endif

#ifdef ENABLE_OPC
//------------------------------------------------------------------------------
// Open Pixel Protocol callbacks:

// Callback when a full OPC Message has been received
void cbOpcMessage(uint8_t channel, uint8_t command, uint16_t length, uint8_t* data) {
  if(0 == channel || 1 == channel)
  {
    switch(command)
    {
      case 0x00:
                  for(uint8_t i = 0; i < length/3; i++)
                  {
                    leds[i].r = data[i*3+0];
                    leds[i].g = data[i*3+1];
                    leds[i].b = data[i*3+2];
                  }
                  FastLED.show(brightness);
                  break;
      case 0xFF: break;
    }
  }
}

// Callback when a client is connected
void cbOpcClientConnected(WiFiClient& client) 
{
  Serial.print("::cbOpcClientConnected(): New OPC Client: ");
#if defined(ESP8266) || defined(PARTICLE)
  Serial.println(client.remoteIP());
#else
  Serial.println("(IP Unknown)");
#endif
  displayClock = false;
}

// Callback when a client is disconnected
void cbOpcClientDisconnected(OpcClient& opcClient) {
  Serial.print("::cbOpcClientDisconnected(): Client Disconnected: ");
  Serial.println(opcClient.ipAddress);
  displayClock = true;
  clock.update(true);
  ani.transform(leds, leds_target, NUM_LEDS, true);
}
#endif

//------------------------------------------------------------------------------
//Ticker callback:
void cbTick()
{
  if(0 == leds[0].b) leds[0] = CRGB::Blue;
  else leds[0] = CRGB::Black;

  FastLED.show();
}

//------------------------------------------------------------------------------
//WiFiManager callback:
bool shouldSaveConfig = false;

void cbSaveConfig()
{
  Serial.println("::cbSaveConfig(): Should save config");
  shouldSaveConfig = true;
}

void cbConfigMode(WiFiManager *myWiFiManager)
{
  ticker.attach(0.2, cbTick);
}

enum eWifiState
{
  e_disconnected = 0,
  e_connected,
  e_reconnect_needed,  
};

eWifiState eWiFiConnState = e_disconnected;

// declare the event handler
WiFiEventHandler disconnectedEventHandler;
WiFiEventHandler connectedEventHandler;

// WiFi disconnect handler
// WiFiEventHandler onStationModeDisconnected(std::function<void(const WiFiEventStationModeDisconnected&)>);
void onWiFiDisconnected(const WiFiEventStationModeDisconnected& event)
{
   if(e_disconnected != eWiFiConnState)
   {
     serialTrace.Log(T_ERROR, "WiFi Adapter disconnected from WLAN");
     eWiFiConnState = e_disconnected;
   }
}

// WiFi connect handler
// WiFiEventHandler onStationModeConnected(std::function<void(const WiFiEventStationModeConnected&)>);
void onWiFiConnected(const WiFiEventStationModeConnected& event)
{
  if(e_reconnect_needed != eWiFiConnState)
  {
    serialTrace.Log(T_INFO, "WiFi Adapter connected to WLAN");
    eWiFiConnState = e_reconnect_needed;
  }
}

void SetNewNtp(const char* ntp)
{
  strcpy(ntp_server, ntp);
  
  IPAddress timeServerIP;
  WiFi.hostByName(ntp_server, timeServerIP);
  Serial.print("NTP Server: ");
  Serial.print(ntp_server);
  Serial.print(" - ");
  Serial.println(timeServerIP);
  
  if(timeServerIP == INADDR_NONE)
  {
    serialTrace.Log(T_WARNING, "Unknown NTP - using default");
    strcpy(ntp_server, DEFAULT_NTP);
    WiFi.hostByName(ntp_server, timeServerIP);
    Serial.print("NTP Server: ");
    Serial.print(ntp_server);
    Serial.print(" - ");
    Serial.println(timeServerIP);
  }
   

  serialTrace.Log(T_INFO, "Reinitializing RTC");
  
  // Rtc.setup();
  Ntp.setup(timeServerIP);
  Rtc.setSyncProvider(&Ntp);
  setSyncProvider(getDateTimeFromRTC);
}

void WiFiReconnect()
{
  if(e_reconnect_needed == eWiFiConnState)
  {
    serialTrace.Log(T_INFO, "WiFi reconnected. Trying to establish all connections again ... ");

    SetNewNtp(ntp_server);

    eWiFiConnState = e_connected;
  }
}

#ifdef ENABLE_OPC
//==============================================================================
// OpcServer Init
//==============================================================================
WiFiServer server = WiFiServer(OPC_PORT);
OpcClient opcClients[OPC_MAX_CLIENTS];
uint8_t buffer[OPC_BUFFER_SIZE * OPC_MAX_CLIENTS];
OpcServer opcServer = OpcServer(server, OPC_CHANNEL, opcClients, OPC_MAX_CLIENTS, buffer, OPC_BUFFER_SIZE);
//------------------------------------------------------------------------------
#endif

#ifdef ENABLE_LOCAL_WEBSERVER
//==============================================================================
// WebServer Init
//==============================================================================
#include "WebServer.h"
WebServer wsrv;

// test mode
void toggleTestMode() 
{
  if(!IsTestMode)
  {
    serialTrace.Log(T_INFO, "Enable Test Mode"); 
    IsTestMode = true;
    displayClock = false;
    for(uint8_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i].r = 255;
      leds[i].g = 127;
      leds[i].b = 36;
    }
    ani.transform(leds, leds_target, NUM_LEDS, true);
    FastLED.show(5); // low brightness to reduce current consumption
  }
  else
  {
    IsTestMode = false;
    serialTrace.Log(T_INFO, "Disable Test Mode"); 
    clock.update(true);
    ani.transform(leds, leds_target, NUM_LEDS, true);
    FastLED.show();
    displayClock = true;
  }  
}

void HandleWebSvrReq(WebServer::wsRequest request)
{
  if(!request.HasColorChanged() && !request.HasBrightnessChanged() && !request.HasNtpChanged() && !request.m_bEnableConfigMode && !request.m_bToggleTestMode && !request.HasDialectChanged() && !request.HasNightChanged())
    return;

  bool bSaveConfigChanges = false;
  
  serialTrace.Log(T_DEBUG, "HandleWebSvrReq - user input detected.");

  if(request.HasColorChanged())
  {
    serialTrace.Log(T_DEBUG, "HandleWebSvrReq - clock color change requested.");
    updateClock(request.GetColor()); // does save the new config
  }

  if(request.HasBrightnessChanged())
  {
    serialTrace.Log(T_DEBUG, "HandleWebSvrReq - clock brightness change requested.");
    brightness = request.GetBrightness();
    brightnessDay = request.GetBrightnessDay();
    brightnessNight = request.GetBrightnessNight();
    FastLED.setBrightness(brightness);
    FastLED.show();
    bSaveConfigChanges = true;
  }

  if(request.HasNtpChanged())
  {
    serialTrace.Log(T_DEBUG, "HandleWebSvrReq - Ntp change requested.");
    SetNewNtp(request.GetNtp().c_str());
    bSaveConfigChanges = true;
  }
  
  if(request.m_bEnableConfigMode)
  {
    serialTrace.Log(T_DEBUG, "HandleWebSvrReq - Config Mode requested.");
    IsConfigMode = true;
    bSaveConfigChanges = true;
  }
  
  if(request.m_bToggleTestMode)
  {
    serialTrace.Log(T_DEBUG, "HandleWebSvrReq - Toggle Test Mode requested.");
    toggleTestMode();
  }

  if(request.HasDialectChanged())
  {
    serialTrace.Log(T_DEBUG, "HandleWebSvrReq - Toggle Dailect requested.");
    clock.SetDialect(request.GetDialect());
    clock.update(true);
    bSaveConfigChanges = true;
  }

  if(request.HasNightChanged())
  {
    serialTrace.Log(T_DEBUG, "HandleWebSvrReq - Night change requested.");
    nightStart = request.GetNightStart();
    nightEnd = request.GetNightEnd();
    bSaveConfigChanges = true;
  }

  if(bSaveConfigChanges)
    SaveTheConfig(clock.getColor());
}

//------------------------------------------------------------------------------
#endif

void setup () 
{
  serialTrace.Init(57600);
  serialTrace.Log(T_INFO, "compiled: %s %s", __DATE__, __TIME__); 

#ifdef SMALLCLOCK
  serialTrace.Log(T_INFO, "compiled for SMALL CLOCK"); 
#else
  serialTrace.Log(T_INFO, "compiled for BIG CLOCK");
#endif
  
  //FastLED.setMaxPowerInVoltsAndMilliamps(5,1000); 
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  //FastLED.setCorrection(0xF0FFF7);
  //FastLED.setTemperature(Tungsten40W);

  FastLED.setBrightness(brightness);

  fill_solid( &(leds[0]), NUM_LEDS, CRGB::Black);
  FastLED.show();

  ticker.attach(0.6, cbTick);

  //read configuration from FS json
  Serial.println("mounting FS...");

  CRGB ledcolor;
  ledcolor.r = DEFAULT_RED;
  ledcolor.g = DEFAULT_GREEN;
  ledcolor.b = DEFAULT_BLUE;

  clockDialect = CClockDisplay::eD_Ossi;
  
  if (SPIFFS.begin()) 
  {
    Serial.println("mounted file system");
    ReadTheConfig(ledcolor, clockDialect);
  } 
  else 
  {
    serialTrace.Log(T_ERROR, "failed to mount FS");
  }
  //end read


  WiFiManager wifiManager;
  wifiManager.setAPCallback(cbConfigMode);
  wifiManager.setSaveConfigCallback(cbSaveConfig);

  WiFiManagerParameter custom_ntp_server("server", "NTP server", ntp_server, 50);
  wifiManager.addParameter(&custom_ntp_server);

  WiFiManagerParameter custom_station_name("station", "WiFi station name", station_name, 50);
  wifiManager.addParameter(&custom_station_name);
  
#ifdef ENABLE_BLYNK
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 33);
  wifiManager.addParameter(&custom_blynk_token);
#endif
  //reset settings - for testing
  //wifiManager.resetSettings();

// Debug is enabled by default on Serial. To disable add before autoConnect
#ifndef _DEBUG
  wifiManager.setDebugOutput(false);
#endif

  Serial.print("MAC Adress is ");
  Serial.println(WiFi.macAddress());

  Serial.print("Wifi Station Name is ");
  Serial.println(station_name);
  wifi_station_set_hostname(station_name);

  if(IsConfigMode)
  {
    IsConfigMode = false;
    SaveTheConfig(ledcolor); // we need to reset the config flag

    if (!wifiManager.startConfigPortal("ESPClockAP"))
    {
      serialTrace.Log(T_ERROR, "failed to connect as Wifi client and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
  }
  else
  {
    // default ip will be 192.168.4.1
    wifiManager.setConnectTimeout(60);
    if (!wifiManager.autoConnect("ESPClockAP"))
    {
      serialTrace.Log(T_ERROR, "failed to connect as Wifi client and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
  }
  
  //if you get here you have connected to the WiFi
  serialTrace.Log(T_INFO, "connected...yeey :)");

  // first connection establish, lets initialize the handlers needed for proper reconnect
  disconnectedEventHandler = WiFi.onStationModeDisconnected(&onWiFiDisconnected); 
  connectedEventHandler = WiFi.onStationModeConnected(&onWiFiConnected);

  //read updated parameters
  strcpy(ntp_server, custom_ntp_server.getValue());
  strcpy(station_name, custom_station_name.getValue());
#ifdef ENABLE_BLYNK
  strcpy(blynk_token, custom_blynk_token.getValue());
#endif

  //save the custom parameters to FS
  if (shouldSaveConfig)
  {
    SaveTheConfig(ledcolor);
  }
  
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  

#ifdef ENABLE_BLYNK
  Serial.print("Blynk token: ");
  Serial.println(blynk_token);
#endif

  IPAddress timeServerIP;
  WiFi.hostByName(ntp_server, timeServerIP);
  Serial.print("NTP Server: ");
  Serial.print(ntp_server);
  Serial.print(" - ");
  Serial.println(timeServerIP);

  serialTrace.Log(T_DEBUG, "Start initializing RTC");
  
  Rtc.setup();
  Ntp.setup(timeServerIP);
  Rtc.setSyncProvider(&Ntp);
  setSyncProvider(getDateTimeFromRTC);
  
  serialTrace.Log(T_DEBUG, "RTC set up");

  updateBrightness();
  
  clock.CLOCKSETUP(&(leds_target[0]), NUM_LEDS);
  clock.SetDialect(clockDialect);
  clock.setTimezone(&CE);
  clock.setColor(ledcolor);

#ifdef ENABLE_OPC
  opcServer.setMsgReceivedCallback(cbOpcMessage);
  opcServer.setClientConnectedCallback(cbOpcClientConnected);
  opcServer.setClientDisconnectedCallback(cbOpcClientDisconnected);
  
  opcServer.begin();
  
  Serial.print("\nOPC Server: ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(OPC_PORT);
#endif

#ifdef ENABLE_BLYNK
  serialTrace.Log(T_DEBUG, "Configure Blynk");
  
  Blynk.config(blynk_token);
  serialTrace.Log(T_DEBUG, "Blynk configured");

  if(false == Blynk.connect(BLYNK_CONNECT_TIMEOUT_SEC * 333)) // Blynk.connect is in 3ms units.
    serialTrace.Log(T_WARNING, "No connection to Blync Server after %d seconds. Blynk app will not work before the connection has been extablished.", BLYNK_CONNECT_TIMEOUT_SEC);
  else
    serialTrace.Log(T_INFO, "Connected to Blynk server.");

  BlynkConnTimer.setInterval(60000, reconnectBlynk);
#endif

#ifdef ENABLE_LOCAL_WEBSERVER
  wsrv.Configure();
#endif

  char* temp = ftoa(Rtc.GetTemperature(), 2);
  serialTrace.Log(T_INFO, "Temperature = %s", temp);
  delete [] temp; temp = 0;

  ticker.detach();

  serialTrace.Log(T_DEBUG, "Setup finsihed.");
}



void loop () 
{
  if (timeSet != timeStatus())
  {
    serialTrace.Log(T_ERROR, "Time is not set. The time & date are unknown.");
    //TODO what shall we do in this case? 
  }

  updateBrightness();

  if(displayClock)
  {
    bool changed = clock.update();
    
    ani.transform(leds, leds_target, NUM_LEDS, changed);
    
    FastLED.show();
  }
  else
  {
      // serialTrace.Log(T_ERROR, "loop - Opc Mode - not supported right now????");
      if(IsTestMode)      
        FastLED.show();
  }

#ifdef ENABLE_OPC
  opcServer.process();
#endif

#ifdef ENABLE_BLYNK
  if (true == Blynk.connected())
  {       
     Blynk.run();
  }
  BlynkConnTimer.run();
#endif

#ifdef ENABLE_LOCAL_WEBSERVER
  WebServer::wsRequest req(clock.getColor(), brightnessNight, brightnessDay, brightness, ntp_server, nightStart, nightEnd, clockDialect);
  wsrv.Run(req);
  HandleWebSvrReq(req);
#endif

  // reconnect WiFiManager, if needed
  WiFiReconnect();
  
}



