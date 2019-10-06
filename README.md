# Swimming pool Automate
##ESP32 dual core version
###Improvements as compared to ESP8266 version:

* Data separated from code.
	* Data stored in files in Flash SPIFFS files.
	* Configuration data stored in a JSON file parsed at boot time.
	* WEB pages stored as HTML files.
	* Persistant data stored in EEPROM thanks to Preference library.
* HTML code is dynamically changed to reflect current status
* HTML code is responsive thanks to Bootstrat Javscript libraries
* Summer/Winter Time adjustement with NTP client.

##Environement
* Compatible de: AsyncTCP v1.0.0 et ESP32 v1.0.0
* Compiler avec:
	* Arduino IDE 1.8.9 ou 1.8.10 ou PR-beta1.9-BUILD-107
	* Utiliser Board ESP 1.0.0 (ESP32)
