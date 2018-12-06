# Swimming pool Automate
##ESP32 dual core version
###Improvements as compared to ESP8266 vesrion:

* Data separated from code.
	* Data stored in files in Flash SPIFFS files.
	* Configuration data stored in a JSON file parsed at boot time.
	* WEB pages stored as HTML files.
	* Persistant data stored in EEPROM thanks to Preference library.
* HTML code is dynamically changed to reflect current status
* HTML code is responsive thanks to Bootstrat Javscript libraries
* Summer/Winter Time adjustement with NTP client.