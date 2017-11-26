## **espclock**
### **Features**
*	OPC control
*	Blync control
*	Color and Brightness Selection
*	Day-Night Switch
*	Compliler switch for OPC, listening on port 7890
*	Compiler switch for Blynk, connects to blync-cloud.com:8442
*	Level based serial tracing for debugging purposes, compiler switch for runtime mode
*	Configuration permanently safed on ESP, not on only Blynk server
*	Locally hosted WebServer incl. compiler switch, listens on port 80 (http)
*	Automatic web site refresh
*	Enables Configuration-Access-Point on request
*	Temperature is shown on web page
*	TestMode, all leds, weak brightness
*	HTML5 Color Picker added, does not work with older browser versions
*	Supports three german dialects
*	Select day and night time on web server
*	Configurable station name
*	Display current time on the web page
*	Inverse Clock Display
*	TV simulation for times where no one is at home
*	Sun Rise Mode: Slowing increasing brightness while switching into day mode
*	Night color selection, different to day color
*	Button for pre-defined color, warm-white (255, 127, 36)
*	Time based start and end of TV simulation including random variation of both times

### **Todo:**
*	Individual Sun-Rise selection for every day of the week
*	Optimize programming (webserver code structuring, reduce globals and optimize for speed)
*	Weak glowing initials or name in unused leds, always, birthday only???
*	Second pulse in one of the unused leds, indvidual color or just red

### **Fixed:** 
*	Execute Blynk only after the server connection has been established
*	Blynk and NTP reconnect after WLAN outage
*	Clock Display fix in case of disconnected WLAN
*	One common define for Small/Big Clock
*	Half To/Past Display fix
*	Reads older config file versions w/o cores :)
*	Wrong day/night switch at certain conditions
*	Several minor fixes

### **Notes:**
1.	This clock does not require internet access.
	Depending on the enabled services (e.g. NTP, OPC, Blync) different ports are opened and connections established.
	In case the local router or any other device in the LAN can act as ntp server and OPC and Blynk disabled 
	no incoming or outgoing internet access is needed.
2.	The first start and every subsequent start where no WLAN connection can be established, the clock will start its own
	Access Point presenting the configuration page.
	If need be, the configuration Access Point can be triggered at any time via the clock's webserver plus a hardware reset.
3.	TVSim and SunRise are running with nearly maximum brightness. Make sure your power supply can handle it.
	