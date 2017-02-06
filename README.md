# espclock
# Added: 
#       Compliler switch for OPC, listening on port 7890
#       Compiler switch for Blynk, connects to blync-cloud.com:8442
#       Level basd serial tracing for debugging purposes, compiler switch for runtime mode
#       Complete configuration gets permanently safed on clock, not only Blynk server  
#       Locally hosted WebServer incl. compiler switch, listens on port 80 (http)
#       Automatic web site refresh
#       Enable Config AP on request
#       Temperature is shown on web page
#       TestMode, all leds, weak brightness
#       HTML5 Color Picker added, does not work with older browser versions.
#       Choose german dialect on web server
#       Select night hours on web server
#       website beautifying
# 	    Configurable station name on config page
# Todo:
#		pick differnt color for the night
#		tv imitation in times where no one is at home?
#       Display current time on the web page as well
#		optimize programming (webserver code structuring, reduce globals and optimize for speed)
#       weak glowing initials or name in unused leds, always, birthday only???
#       second pulse in one of the unused leds, indvidual color or just red
#       Rainbow, dropping rain, falling snow, whatever
# Fixed: 
#       Execute Blynk only after the server connection has been established
#       Blynk and NTP reconnect after WLAN outage
#       Clock Display fix in case of disconnected WLAN
#       One common define for Small Clock A0:20:A6:0A:D0:34 
# Note:
#       If no WLAN connection available at start-up an Access Point and the Configuration WebServer gets started.
#       Depending on the enabled sevice different ports are opened or connections established.
#       In case the local router or any other device in the intranet can act as ntp server and OPC and Blynk functionality
#       are disabled no incoming or outgoing internet access is required.
#       If so the configuration can only be changed having a web client being connected to the same local network. For a clock this is good enough.