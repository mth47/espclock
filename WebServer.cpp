#include "WebServer.h"
#include "GlobalsAndDefines.h"


WebServer::WebServer() 
{
   ulReqcount=0;         // setup globals for Webserver
   ulReconncount=0;
}

WebServer::~WebServer() 
{
  server.stop();
}  

// blocks until connected to WLAN
void WebServer::Configure()
{
   ulReconncount++;
   
   while (WiFi.status() != WL_CONNECTED) 
   {
     delay(500);
   }
   
   // Start the server
   server.begin();
}

/// Complete web server //////////////////////////////////////////////////////////////////
void WebServer::Run(wsRequest& request)
{
  
  // check if WLAN is connected
  if (WiFi.status() != WL_CONNECTED)
  {
    return;
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }
  
  // Wait until the client sends some data
  serialTrace.Log(T_INFO, "WebServer - New client connected");

  unsigned long ultimeout = millis()+250;

  while(!client.available() && (millis()<ultimeout) )
  {
    delay(1);
  }
  
  if(millis()>ultimeout) 
  { 
    serialTrace.Log(T_INFO, "WebServer - client connection time-out!");
    return; 
  }
  
  // Read the first line of the request
  String sRequest = client.readStringUntil('\r');
  serialTrace.Log(T_DEBUG, "WebServer - Request = %s", sRequest.c_str());
  client.flush();  
  
  // stop client, if request is empty
  if(sRequest=="")
  {
    serialTrace.Log(T_ERROR, "WebServer - empty request! - stopping client");
    client.stop();
    return;
  }

  // get path; end of path is either space or ?
  // Syntax is e.g. GET /?pin=MOTOR1STOP HTTP/1.1
  String sPath="",sParam="", sCmd="";
  String sGetstart="GET ";
  int iStart,iEndSpace,iEndQuest;
  iStart = sRequest.indexOf(sGetstart);
  if (iStart>=0)
  {
    iStart+=+sGetstart.length();
    iEndSpace = sRequest.indexOf(" ",iStart);
    iEndQuest = sRequest.indexOf("?",iStart);

    // are there parameters?
    if(iEndSpace>0)
    {
      if(iEndQuest>0)
      {
        // there are parameters
        sPath  = sRequest.substring(iStart,iEndQuest);
        sParam = sRequest.substring(iEndQuest,iEndSpace);
      }
      else
      {
        // NO parameters
        sPath  = sRequest.substring(iStart,iEndSpace);
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // output parameters to serial, you may connect e.g. an Arduino and react on it or for debugging only
  ///////////////////////////////////////////////////////////////////////////////
  if(sParam.length()>0)
  {
    // int iEqu=sParam.indexOf("=");
    //if(iEqu>=0)
    // {
      // sCmd = sParam.substring(iEqu+1,sParam.length());
      sCmd = sParam.substring(1,sParam.length());
      serialTrace.Log(T_DEBUG, "WebServer - sCmd = %s", sCmd.c_str());
    // }
  }

  ///////////////////////////
  // format the html response
  ////////////////////////////
  String sResponse, sHeader;
  ////////////////////////
  // 404 for non-matching path
  ////////////////////////////
  if(sPath!="/")
  {
    serialTrace.Log(T_ERROR, "WebServer - unknown request. Sending 404.");
    sResponse="<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
    sHeader  = "HTTP/1.1 404 Not found\r\n"
               "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"
               "\r\n";
  }
  ///////////////////////
  // format the html page
  ///////////////////////
  else
  {    
   
    CRGB color = clock.getColor();
    ulReqcount++;
    sResponse  =  F("<html><head>"
                      "<title>WordClock Control</title>");
    sResponse  += htmlStyleCSS;
    
    //AddColorPickerScript(sResponse, "#FF7F24", "colorpicker");
    
    sResponse  += F("<base href=\"/\">"
                  "<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1/jquery.min.js\"></script>"
                  "<script language=\"javascript\" type=\"text/javascript\">"
                  "$(document).ready(function(){"
                  "$(\"#check_slider\").click(function() {"
                  "$('tr:nth-child(2)').toggle(\"slow\");"
                  "$('tr:nth-child(3)').toggle(\"slow\");"
                  "$('tr:nth-child(4)').toggle(\"slow\");"
                  "$('tr:nth-child(5)').toggle();"
                  "});"
                  "$(\"#check_picker\").click(function() {"
                  "$('tr:nth-child(5)').toggle(\"slow\");"
                  "$('tr:nth-child(2)').toggle();"
                  "$('tr:nth-child(3)').toggle();"
                  "$('tr:nth-child(4)').toggle();"
                  "});"
                  "});"
                  "</script>"
                  "</head><body bgcolor=white>"
                  // this color pciker does only work with jQuery whcih requires a jquery library
                  //"<script src=\"https://cdnjs.cloudflare.com/ajax/libs/spectrum/1.8.0/spectrum.min.js\"></script>"
                  //"<link rel=\"stylesheet\" type=\"text/css\" href=\"https://cdnjs.cloudflare.com/ajax/libs/spectrum/1.8.0/spectrum.min.css\">"
                  "<font color=\"#000000\">"
                  "<meta name=\"viewport\" content=\"width=device-width, height=device-height initial-scale=1.0, user-scalable=yes\">"
                  "<h1>");
    sResponse  += station_name;
    sResponse += F(" UI</h1><h3 style=\"color:#787878\;\">");
    sResponse  += clock.GetClockString();
    sResponse  += F("</h3>"
                  "<FONT SIZE=+1>"
                  // "<form action=\"/\" autocomplete=\"on\">"
                  //"<form action=\"/\" autocomplete=\"on\">"
                  // "<form action=\"window.location.assign(location.origin)\" autocomplete=\"on\">"
                  // "<form action=\"/\" autocomplete=\"on\" onsubmit=\"window.location.href = location.origin; window.setTimeout(window.location.reload(true), 1000);\">"
                  //"<form action=\"/\" autocomplete=\"on\" onsubmit=\"setTimeout(function() {window.location.href = location.origin; setTimeout(function() {window.location.reload(true)}, 200)}, 1000);\">"
                  // "<form action=\"\" autocomplete=\"on\" onsubmit=\"setTimeout(function() {window.location.href = location.origin; setTimeout(function() {window.location.reload(true)}, 200)}, 1000);\">"
                  // "<table frame=\"void\">");
                  "<form autocomplete=\"on\">"
                  "<table border=\"0\">"
                  "<tr><td width=\"120px\">"                  
                  "<input type=\"radio\" id=\"check_slider\" name=\"ColorChoice\" value=\"Slider\" checked=\"checked\">"
                  "<label for=\"check_slider\"> RGB Slider </label> "
                  "</td><td width=\"200px\">"
                  "<input type=\"radio\" id=\"check_picker\" name=\"ColorChoice\" value=\"Picker\">"
                  "<label for=\"check_picker\"> RGB Color Picker</label>"
                  "</td><td width=\"60px\"></td></tr>");

    
    // add color picker, FF7F24 == warm white
    // AddColorPicker(sResponse, "colorpicker");

    // red slider
    AddSliderControl(sResponse, "#FF0000", "Red", "arRED", "arR", 0, 255, color.r, "aiRED", "aiR");
    
    // green slider
    AddSliderControl(sResponse, "#00FF00", "Green", "arGREEN", "arG", 0, 255, color.g, "aiGREEN", "aiG");

    // blue slider
    AddSliderControl(sResponse, "#0000FF", "Blue", "arBLUE", "arB", 0, 255, color.b, "aiBLUE", "aiB");

    //sResponse +=  F("<!-- emtpy row--><tr style=\"border-bottom:1pt solid " TABLE_BORDER_COLOR ";\"><td><br></td><td><br></td><td><br></td></tr>");

    // ----------- color picker
    sResponse +=  F("<tr style=\"display: none;\"><td></td><td><input type=\"color\" name=\"colorPicker\" value=\"");

    String c = RGBToHexString(color);
    serialTrace.Log(T_DEBUG, "WebServer - Current RGB in Hex = %s", c.c_str());
    sResponse +=  c;
    
    sResponse +=  F("\"></td><td></td></tr>");   
    // ----------- end color picker
    
    //sResponse  += F("</table><BR><table border=\"0\"><!-- set column size--><tr><td width=\"120px\"></td><td width=\"200px\"></td><td width=\"60px\"></td></tr>");
    sResponse +=  F("<!-- emtpy row--><tr><td><br></td><td><br></td><td><br></td></tr>");

    // night start hour
    int8_t h = GetHourOfDayMin(nightStart);
    int8_t m = GetMinOfDayMin(nightStart);
    AddTwoNumberFields(sResponse, " Night Start ", "nightStartH", "nStartH", h, "nightStartM", "nStartM", m, true, true);
    // night end hour
    h = GetHourOfDayMin(nightEnd);
    m = GetMinOfDayMin(nightEnd);
    AddTwoNumberFields(sResponse, " Night End ", "nightEndH", "nEndH", h, "nightEndM", "nEndM", m, true, true);
    
    // brightness slider
    AddSliderControl(sResponse, "#000000", "Brightness", "arBright", "arBR", 0, 100, brightness, "aiBRIGHT", "aiBR");

   
    // brightnessDay slider
    AddSliderControl(sResponse, ((bIsDay) ? "#000000" : "#787878"), "Brightness Day", "arBrightD", "arBRD", 0, 100, brightnessDay, "aiBRIGHTD", "aiBRD");

    // brightnessNight slider
    AddSliderControl(sResponse, ((bIsDay) ? "#787878" : "#000000"), "Brightness Night", "arBrightN", "arBRN", 0, 100, brightnessNight, "aiBRIGHTN", "aiBRN");

    //sResponse  += F("</table><BR><table border=\"0\"><!-- set column size--><tr><td width=\"120px\"></td><td width=\"200px\"></td><td width=\"60px\"></td></tr>");
    sResponse +=  F("<!-- emtpy row--><tr><td><br></td><td><br></td><td><br></td></tr>");
    
    // ntp server
    AddTextField(sResponse, "#000000", "NTP Server", "aiNTPSvr", "aiNTP", ntp_server);

    // temperature
    AddNumberField(sResponse,"Temperature",  "", "", Rtc.GetTemperature(), true, false);
   
    // apply button
    sResponse +=  F("<!-- emtpy row--><tr><td><br/></td><td><br/></td><td><br/></td></tr>"
                  "<tr><td></td><td><input type=\"submit\" data-inline=\"true\" value=\"Apply\"></td><td></td></tr>"      
                  // "<tr><td></td><td><input type=\"button\" class=\"button\" onClick=\"location.reload(true)\" value=\"Refresh\"></td><td></td></tr>"                   
                  "<!-- emtpy row--><tr><td><br></td><td><br></td><td><br></td></tr>"
                  "<!-- emtpy row--><tr><td><br></td><td><br></td><td><br></td></tr>");
    // void AddCheckBox(String &msg, String label, String fieldName, String fieldId, String value, String currentValueColor)
    if(IsConfigMode)
      AddCheckBox(sResponse, "Config Mode after restart", "CheckConfig", "ChkCfg", "CONFIGON", "ON", "#787878");
    else
      AddCheckBox(sResponse, "Config Mode after restart", "CheckConfig", "ChkCfg", "CONFIGON", "OFF", "#787878");
 
    //AddButton(sResponse, "Config Mode after restart", "CONFIGON", "Set");
    if(IsTestMode)
      AddCheckBox(sResponse, "Toggle Test Mode", "TestMode", "TsM", "TEST", "ON", "#787878");
    else
      AddCheckBox(sResponse, "Toggle Test Mode", "TestMode", "TsM", "TEST", "OFF", "#787878");
    //AddButton(sResponse, "Test Mode", "TEST", "Toggle");

    // add the radios for the three languages, one per column
    // AddCheckBox(sResponse, "Toggle Dialect", "ToggleDialet", "Dia", "DIA", (clock.IsSouthGermanDialect() ? "SOUTH and EAST" : "WEST") );
    sResponse +=  "<tr><td>";
    AddRadioButton(sResponse, "Ostdeutsch", "dialect", "dOst", "Ost", clock.IsOssiDialect());
    sResponse +=  "</td><td>";
    AddRadioButton(sResponse, "Westdeutsch", "dialect", "dWest", "West", clock.IsWessiDialect());
    sResponse +=  "</td><td>";
    AddRadioButton(sResponse, "RheinRhur", "dialect", "dRR", "RR", clock.IsRheinRuhrDialect());
    sResponse +=  "</td></tr>";
    
    
    sResponse +=  F("</table>"
                  "</form>"
                  "<FONT SIZE=-3>"
                  "<BR>"
                  "<BR>"
                  "WordClock designed by J.Willnecker<BR>"
                  "WebServer implemented by M.Thomas<BR>");

    //////////////////////
    // react on parameters
    //////////////////////
    if (sCmd.length()>0 && sCmd!="r")
    {
      // write received command to html page
    #ifdef _DEBUG
      sResponse += "Command: " + sCmd + "<BR>";
    #endif
      // read parameters
      // ColorChoice=Slider&arRED=0&aiRED=0&arGREEN=0&aiGREEN=0&arBLUE=255&aiBLUE=255&colorPicker=%230000ff&arBright=50&aiBRIGHT=50&arBrightD=50&aiBRIGHTD=50&arBrightN=5&aiBRIGHTN=5&aiNTPSvr=fritz.box
      //SetIntFn = &wsRequest::SetColorRed;
      
      bool bSlider = UseSlider(sCmd, "ColorChoice=");
      
      if(bSlider)
      {
        ReadSliderValue(sCmd, "arRED=", request, &wsRequest::SetColorRed);
        ReadSliderValue(sCmd, "arGREEN=", request, &wsRequest::SetColorGreen);
        ReadSliderValue(sCmd, "arBLUE=", request, &wsRequest::SetColorBlue);
        // skip color picker
        SkipValue(sCmd);
      }
      // reading color picker can only be exclusive, RGB slider or color picker, as I cannot update its value dynamically on the client
      else
      { 
        // skip sliders, two values for every slider
        SkipValue(sCmd);
        SkipValue(sCmd);
        SkipValue(sCmd);
        SkipValue(sCmd);
        SkipValue(sCmd);
        SkipValue(sCmd);
        ReadColorPicker(sCmd, "colorPicker=", request);
      }

      // night start hour
      h = 0; m = 0;
      ReadNumberField(sCmd, "nightStartH=", h);
      ReadNumberField(sCmd, "nightStartM=", m);
      request.SetNightStart(GetMinOfTime(h,m));
      
      // night end hour
      ReadNumberField(sCmd, "nightEndH=", h);
      ReadNumberField(sCmd, "nightEndM=", m);
      request.SetNightEnd(GetMinOfTime(h,m));
      
      ReadSliderValue(sCmd, "arBright=", request, &wsRequest::SetBrightness);
      ReadSliderValue(sCmd, "arBrightD=", request, &wsRequest::SetBrightnessDay);
      ReadSliderValue(sCmd, "arBrightN=", request, &wsRequest::SetBrightnessNight);

      ReadTextField(sCmd, "aiNTPSvr=", request, &wsRequest::SetNtp);
      
      if(sCmd.indexOf("CONFIGON")>=0)
      {
        serialTrace.Log(T_DEBUG, "WebServer - CONFIGON");
        request.m_bEnableConfigMode = true;
      }

      if(sCmd.indexOf("TEST")>=0)
      {
        serialTrace.Log(T_DEBUG, "WebServer - TEST");
        request.m_bToggleTestMode = true;
      }           

      String param = ReadValueAsString(sCmd, "dialect=");
      serialTrace.Log(T_DEBUG, "WebServer - new dialect == %s", param.c_str());
      if (String("West") == param)
        request.SetDialect(CClockDisplay::eD_Wessi);
      else if (String("RR") == param)
        request.SetDialect(CClockDisplay::eD_RheinRuhr);
      else
        request.SetDialect(CClockDisplay::eD_Ossi);
        
    } // end sCmd.length()>0

    sResponse += "<FONT SIZE=-2>"
                 "<BR>clicks on page =";
    sResponse += ulReqcount;
    sResponse += " - connections to page =";
    sResponse += ulReconncount;
    sResponse += "<BR>";
    //sResponse += "<BR><!--[if IE]>";
    sResponse += autoRefreshScript;
    //sResponse += "<![endif]-->;</body></html>";
    sResponse += "</body></html>";
    sHeader  = "HTTP/1.1 200 OK\r\n"
               "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"
               "\r\n";
  }

  // Send the response to the client
  client.print(sHeader);

  // the response is to big to be send at once, so we need to devide it

  #define RESP_CHUNK_IN_BYTES 1000
  #define RESP_CHUNK_RETRY_CNT 10
  #define ADD_DELAY_MS 5
  size_t bytes2Send = 0;
  size_t sentBytes = 0, tmp_size = 0;
  size_t respSize = sResponse.length();
  int8_t cnt  = RESP_CHUNK_RETRY_CNT;
  while(sentBytes < respSize)
  {
    bytes2Send = respSize - sentBytes;
    tmp_size = client.write(sResponse.c_str() + sentBytes, (bytes2Send <= RESP_CHUNK_IN_BYTES) ? bytes2Send : RESP_CHUNK_IN_BYTES);
    if(0 == tmp_size)
    {
      --cnt;
      serialTrace.Log(T_DEBUG, "WebServer - Respone: Failed to send '%d' of '%d' bytes (Attempt: %d)", bytes2Send, respSize, (RESP_CHUNK_RETRY_CNT - cnt));
      if(0 >= cnt)
      {
        serialTrace.Log(T_DEBUG, "WebServer - Respone: Stop trying. Client dead?");
        break;
      }
      delay (ADD_DELAY_MS);
    }
    sentBytes += tmp_size;
    //client.print(sResponse);
    serialTrace.Log(T_DEBUG, "WebServer - Respone: '%d' bytes of '%d' sent", sentBytes, respSize);
  }
  
  // and stop the client
  client.stop();

  serialTrace.Log(T_INFO, "WebServer - Client disonnected");
  
  return;
}
/// END of complete web server //////////////////////////////////////////////////////////////////

String WebServer::RGBToHexString(CRGB rgb)
{
  String c = "#" + ColorValueToHex(rgb.r);
  c += ColorValueToHex(rgb.g);
  c += ColorValueToHex(rgb.b);
  return c;
}

String WebServer::ColorValueToHex(int8_t color)
{
  char* tmp = new char[10];
  if(!tmp)
    return String("\0");

  memset(tmp, 0, sizeof(char) * 10);

  // looks like printf is reading 32bit, even out of a 8 bit integer.
  int c32 = color;
  sprintf(tmp,"%02x", c32);
  // the numbers are larger than two bytes in case of FF. We need to truncate them.
  tmp[2] = 0;
  
  serialTrace.Log(T_DEBUG, "WebServer - color tmp = %s", tmp);

  String hs = tmp;
  delete [] tmp; tmp = 0;
  return hs;
}

int8_t WebServer::HexColorValueToInt(String hc)
{
  int8_t val = 0;

  for(int8_t i = 0; i <=1; ++i)
  {
    if('f' == hc[i] || 'F' == hc[i])
     val += (0xF<<(4*(1-i)));
    else if('e' == hc[i] || 'E' == hc[i])
     val += (0xE<<(4*(1-i)));
    else if('d' == hc[i] || 'D' == hc[i])
     val += (0xD<<(4*(1-i)));
    else if('c' == hc[i] || 'C' == hc[i])
     val += (0xC<<(4*(1-i)));
    else if('b' == hc[i] || 'B' == hc[i])
     val += (0xB<<(4*(1-i)));
    else if('a' == hc[i] || 'A' == hc[i])
     val += (0xA<<(4*(1-i)));
    else if('9' == hc[i])
     val += (0x9<<(4*(1-i)));
    else if('8' == hc[i])
     val += (0x8<<(4*(1-i)));
    else if('7' == hc[i])
     val += (0x7<<(4*(1-i)));
    else if('6' == hc[i])
     val += (0x6<<(4*(1-i)));
    else if('5' == hc[i])
     val += (0x5<<(4*(1-i)));
    else if('4' == hc[i])
     val += (0x4<<(4*(1-i)));
    else if('3' == hc[i])
     val += (0x3<<(4*(1-i)));
    else if('2' == hc[i])
     val += (0x2<<(4*(1-i)));
    else if('1' == hc[i])
     val += (0x1<<(4*(1-i)));
  }
        
  return val;
}

/*void WebServer::AddColorPickerScript(String &msg, String htmlColor, String Id)
{
  msg += "<script  type=\"text/javascript\" >"
         "$(\"#";
  msg += Id;
  msg += "\").spectrum({"
         "color: \"";
  msg += htmlColor;
  msg += "\""    
         "});"
         "</script>";
}
         
void WebServer::AddColorPicker(String &msg, String Id)
{
  msg += "<tr><td></td><td>"  
         "<type=\"text\" input id=\"";
  msg += Id;
  msg += "\"/>";
  AddColorPickerScript(msg, "#FF7F24", Id);
  msg += "</td><td></td></tr>";
}*/

void WebServer::AddSliderControl(String &msg, String htmlColor, String label, String rangeName, String rangeId, int16_t minValue, int16_t maxValue, int16_t curColor, String inputName, String inputId)
{
    msg += "<tr><td><span style=\"color:";
    msg += htmlColor;
    msg += "\">";
    msg += label;
    msg += "</span></td><td><input type=\"range\" name=\"";
    msg += rangeName;
    msg += "\" id=\"";
    msg += rangeId;
    msg += "\" min=\"";
    msg += minValue;
    msg += "\" max=\"";
    msg += maxValue;
    msg += "\" value=\"";
    msg +=  curColor;
    msg += "\" onchange=\"this.form.";
    msg += inputName;
    msg += ".value=this.value\">"
           "</td><td><input type=\"number\" id=\"";
    msg += inputId;
    msg += "\" name=\"";
    msg += inputName;
    msg += "\" min=\"";
    msg += minValue;
    msg += "\" max=\"";
    msg += maxValue;
    msg += "\" value=\"";
    msg +=  curColor;                    
    msg += "\" oninput=\"this.form.";
    msg += rangeName;
    msg += ".value=this.value\"></td></tr>";
}

void WebServer::AddTextField(String &msg, String htmlColor, String label, String fieldName, String fieldId, String fieldValue, bool bTable)
{
    if(bTable) msg += "<tr><td>";
    msg += "<span style=\"color:";
    msg += htmlColor;
    msg += "\">";
    msg += label;
    msg += "</span>";
    if(bTable) msg += "</td><td>";
    msg += "<input type=\"text\" id=\"";
    msg += fieldId;
    msg += "\" name=\"";
    msg += fieldName;
    msg += "\" value=\"";
    msg += fieldValue;
    msg += "\">";
    if(bTable) msg += "</td><td></td></tr>";
}

void WebServer::AddNumberField(String &msg, String label, String fieldName, String fieldId, float fieldValue, bool bTable, bool bEdit)
{
    if(bTable) msg += "<tr><td>";
    msg += "<span>";
    msg += label;
    msg += "</span>";
    if(bTable) msg += "</td><td>";
    if(bEdit)
    {
      msg += "<input type=\"number\" id=\"";
      msg += fieldId;
      msg += "\" name=\"";
      msg += fieldName;
      msg += "\" value=\"";
      msg += fieldValue;
      msg += "\">";
    }
    else
    {
      msg += "<input type=\"number\" value=\"";
      msg += fieldValue;
      msg += "\" disabled>";
    }
    if(bTable) msg += "</td><td></td></tr>";
}

void WebServer::AddTwoNumberFields(String &msg, String label, String fieldName1, String fieldId1, int8_t fieldValue1, String fieldName2, String fieldId2, int8_t fieldValue2, bool bTable, bool bEdit)
{
    if(bTable) msg += "<tr><td>";
    msg += "<span>";
    msg += label;
    msg += "</span>";
    if(bTable) msg += "</td><td>";
    if(bEdit)
    {
      msg += "<input type=\"number\" id=\"";
      msg += fieldId1;
      msg += "\" name=\"";
      msg += fieldName1;
      msg += "\" value=\"";
      msg += fieldValue1;
      msg += "\">";
    }
    else
    {
      msg += "<input type=\"number\" value=\"";
      msg += fieldValue1;
      msg += "\" disabled>";
    }
    
    if(bTable) 
      msg += "</td><td>";
    else
      msg += " ";
      
    if(bEdit)
    {
      msg += "<input type=\"number\" id=\"";
      msg += fieldId2;
      msg += "\" name=\"";
      msg += fieldName2;
      msg += "\" value=\"";
      msg += fieldValue2;
      msg += "\">";
    }
    else
    {
      msg += "<input type=\"number\" value=\"";
      msg += fieldValue2;
      msg += "\" disabled>";
    }

    if(bTable) msg += "</td></tr>";
}

void WebServer::AddButton(String &msg, String label, String fieldName, String buttonValue)
{
    msg += "<tr><td>";
    msg += label;
    msg += "</td><td><a href=\"?pin=";
    msg += fieldName;
    msg += "\"><button class=\"button\">";
    msg += buttonValue;
    msg += "</button></a></td><td></td></tr>";
}

void WebServer::AddCheckBox(String &msg, String label, String fieldName, String fieldId, String value, String currentValue, String currentValueColor)
{
    msg += "<tr><td><input type=\"checkbox\" id=\"";
    msg += fieldName;
    msg += "\" name=\"";
    msg += fieldId;
    msg += "\" value=\"";
    msg += value;
    msg += "\"></td><td>";
    msg += label;
    msg += "</td><td style=\"color:";
    msg += currentValueColor;
    msg += "\">Currently selected: '";
    msg += currentValue;
    msg += "'</td></tr>";
}

void WebServer::AddRadioButton(String &msg, String label, String fieldName, String fieldId, String value, bool bChecked)
{
    msg += "<input type=\"radio\" id=\"";
    msg += fieldId; 
    msg += "\" name=\"";
    msg += fieldName;
    msg += "\" value=\"";
    msg += value;
    if(bChecked)
      msg += "\" checked=\"checked\">";
    else
      msg += "\">";
    msg += "<label for=\"";
    msg += fieldId;
    msg += "\"> ";
    msg += label;
    msg += "</label>";
}

void WebServer::ReadSliderValue(String& sCmd, const char* paramLabel, wsRequest& request, SetIntFn fn)
{
  String param = ReadValueAsString(sCmd, paramLabel);
  if(String("\0") != param)
  {
    (request.*fn) (param.toInt());
  }

  SkipValue(sCmd);
}

// ColorChoice=Slider ( or Picker)
bool WebServer::UseSlider(String& sCmd, const char* paramLabel)
{
  String param = ReadValueAsString(sCmd, paramLabel);
  if(String("Slider") == param)
    return true;

  return false;
}

void WebServer::ReadTextField(String& sCmd, const char* paramLabel, wsRequest& request, SetStringFn fn)
{
   String param = ReadValueAsString(sCmd, paramLabel);
   if(String("\0") != param)
   {
     (request.*fn) (param);
   }
}

void WebServer::ReadNumberField(String& sCmd, const char* paramLabel, wsRequest& request, SetIntFn fn)
{
   String param = ReadValueAsString(sCmd, paramLabel);
   if(String("\0") != param)
   {
     (request.*fn) (param.toInt());
   }
}

void WebServer::ReadNumberField(String& sCmd, const char* paramLabel, int8_t& num)
{
   String param = ReadValueAsString(sCmd, paramLabel);
   if(String("\0") != param)
   {
     num = param.toInt();
   }
}

//colorPicker=%237c3400 == #7c3400
void WebServer::ReadColorPicker(String& sCmd, const char* paramLabel, wsRequest& request)
{
  String param = ReadValueAsString(sCmd, paramLabel);
  if(String("\0") != param)
  {
    serialTrace.Log(T_DEBUG, "WebServer - Hex Color = %s", param.c_str());
    serialTrace.Log(T_DEBUG, "WebServer - Red Hex Color = %s", param.substring(3, 5).c_str());
    request.SetColorRed(HexColorValueToInt(param.substring(3, 5).c_str()));
    serialTrace.Log(T_DEBUG, "WebServer - Green Hex Color = %s", param.substring(5, 7).c_str());
    request.SetColorGreen(HexColorValueToInt(param.substring(5, 7).c_str()));
    serialTrace.Log(T_DEBUG, "WebServer - Blue Hex Color = %s",  param.substring(7, 9).c_str());
    request.SetColorBlue(HexColorValueToInt( param.substring(7, 9).c_str()));
    serialTrace.Log(T_DEBUG, "WebServer - RGB Color = %d %d %d", request.GetColor().r, request.GetColor().g, request.GetColor().b);
  }     
}

String WebServer::ReadValueAsString(String& sCmd, const char* paramLabel)
{
  int16_t idx = 0;
  if((idx = sCmd.indexOf(paramLabel))>=0)
  {
    // serialTrace.Log(T_DEBUG, "WebServer - %s idx = %d", paramLabel, idx);
    idx+=strlen(paramLabel);
    // serialTrace.Log(T_DEBUG, "WebServer - %s idx = %d", paramLabel, idx);
    int16_t idx2 = sCmd.indexOf("&");
    // serialTrace.Log(T_DEBUG, "WebServer - %s idx2 = %d", paramLabel, idx2);
    if(-1 == idx2) // end of string sCmd reached
    {
      idx2 = sCmd.indexOf("?r"); // looking for end tag
      // serialTrace.Log(T_DEBUG, "WebServer - %s idx2 = %d", paramLabel, idx2);
      if(-1 == idx2) // end of string sCmd reached
      {
        idx2 = sCmd.length();
        // serialTrace.Log(T_DEBUG, "WebServer - %s idx2 = %d", paramLabel, idx2);
      }
    }
    
    String param = sCmd.substring(idx, idx2);
    serialTrace.Log(T_DEBUG, "WebServer - %s %s", paramLabel, param.c_str());
    // skip read values
    sCmd = sCmd.substring(idx2+1);
    // serialTrace.Log(T_DEBUG, "WebServer - Remaining Cmd %s", sCmd.c_str());
    return param;
  }
  return String("\0");
}

void WebServer::SkipValue(String& sCmd)
{
  int16_t idx = sCmd.indexOf("&");
  if(-1 == idx) // end of string sCmd reached
  {
    idx = sCmd.indexOf("?r"); // looking for end tag
    if(-1 == idx) // end of string sCmd reached
    {
       idx = sCmd.length()-3;
    }
  }
  
  // skip
  sCmd = sCmd.substring(idx+1);
  serialTrace.Log(T_DEBUG, "WebServer - Remaining Cmd %s", sCmd.c_str());
}


