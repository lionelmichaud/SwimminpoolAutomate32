
// VERSIONS HISTORY

// VERSION 0.1.0
//  Création
//

//#define DEBUG
#define VERBOSE // Infos de version et de connection WiFi
#define ECHO    // Echo toutes les commande reçues de l'Arduino vers l'Arduino après décodage
#define NTP     // Récupération du temps NTP
#define NTP_OFFSET 7200 // 3600 = 1h en hiver ; 7200 = 2h en été
#define NTP_PERIOD 30000 // milisecondes

// VERSION NUMBER
#define SOFTWARE "ESP32_Automat"
#define VERSION "0.1.0"

// IDX of Domoticz devices
#define idx_waterTemp 48
#define idx_airTemp   49
#define idx_automate  50
#define idx_posVolet  51

// how many samples to take and average
#define NUMSAMPLES 10

#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
//#include <ESP8266HTTPClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
//#include <ESP8266HTTPUpdateServer.h>
//#include <ArduinoOTA.h>
//#include <SimpleTimer.h>

// USB serial line bitrate
#define USBSERIAL_BITRATE 19200

// client NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", NTP_OFFSET, NTP_PERIOD);

// serveur WEB
//ESP8266WebServer server(80);
//ESP8266HTTPUpdateServer httpUpdater;

// paramètres Wi-Fi
const char* Local_Name    = "esp01_pool_13";
const char* Pool_ssid     = "Pool";
const char* Garage_ssid   = "Garage";
const char* MonWiFi_ssid  = "Mon Wi-Fi";
const char* MonBWiFi_ssid = "Mon-BWi-Fi";
const char* password = "louannetvanessasontmessourisadorees"; // pour tous les réseaux Wi-Fi

const char* Automat_ssid  = "esp01_pool_13";
const char* Automat_pwd = "Levsmsa2";

const char* host = "192.168.1.23"; // "192.168.1.23";
const int   port = 8084;
//const int   watchdog = 60000; // Fréquence d'envoi des données à Domoticz - Frequency of sending data to Domoticz
const int   aDelay = 500; // délai entre lignes envoyées sur Serial
//HTTPClient http;

IPAddress the_IP;
IPAddress AP_IP;
byte mac[6];
int selectedWiFi = -1;
int bestRSSI = -600;
int theRSSI;
String the_IP_String = "";
String the_AP_IP_String = "";
String the_MAC_String = "";
String the_SSID = "";            // a string to hold the SSID
boolean stringComplete = false;  // whether the string is complete
boolean restartFlag = false;     // whether to restart the module (http command

boolean initAirTemp    = true;
boolean initWaterTemp  = true;
float AirTempSamples[NUMSAMPLES];
float WaterTempSamples[NUMSAMPLES];
float AirTempAverage = 0.0;
float WaterTempAverage = 0.0;
uint8_t i_AirTemp = 0;
uint8_t i_WaterTemp = 0;

// Paramètres à envoyer à Domoticz
float AirTemp       = 0.0;
float WaterTemp     = 0.0;
float MaxIntTemp    = 0.0;
float IntTemp       = 0.0;
float Seuil         = 0.0;
float Hysteresis    = 0.0;
float OffsetAir     = 0.0;
float OffsetEau     = 0.0;

String inputString = "";         // a string to hold incoming data
String AutomateString = "";
String VoletString = "";
String SoftwareString = "";
String url = "";

String TimeNTP = "";
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
// constants won't change :
const unsigned long interval_NTP_to_Arduino  =  61000; // interval at which to send NTP time over USB (milliseconds)
// the timer object
SimpleTimer timer;

//--------------------------------------------------------------------
// SETUP
//--------------------------------------------------------------------
void setup() {
  // reserve n bytes for the Strings: to avoid dynamic realocations
  AutomateString.reserve(50);
  VoletString.reserve(50);
  SoftwareString.reserve(50);
  inputString.reserve(200);

  delay(5500);

  // initialize serial:
  Serial.begin(USBSERIAL_BITRATE);
  delay(10);

#if defined VERBOSE
  Serial.println("");
  Serial.println(F(">--------------------------"));
  delay(aDelay);
  Serial.println(F(">  ESP01 is booting "));
  delay(aDelay);
  Serial.println(F(">--------------------------"));
  delay(aDelay);
  Serial.print(F(">  VERSION = ")); Serial.println(VERSION);
  delay(aDelay);
  Serial.println(F(">  I was compiled " __DATE__ ));
  delay(aDelay);
  Serial.println(F(">--------------------------"));
  delay(aDelay);
#endif

//  Serial.setDebugOutput(false);
//
//  //--------------------------------------------------------------------------------------------------
//  // Set WiFi to station mode + Access Point and disconnect from an AP if it was previously connected
//  //--------------------------------------------------------------------------------------------------
//  WiFi.mode(WIFI_AP_STA);
//  //WiFi.disconnect();
//  delay(1000);
//
//  //-----------------------------------
//  // INITIALIZE SOFT WI-FI ACCESS POINT
//  //-----------------------------------
//  WiFisoftAP();
//  delay(1000);
//
//  //-------------------------
//  // DEMARRER LE SERVEUR WEB
//  //-------------------------
//  WiFiConnect();
//  delay(aDelay);

  //-------------------------
  // DEMARRER LE CLIENT NTP
  //-------------------------
#if defined NTP
  timeClient.begin();
  int TimerNTP = timer.setInterval(interval_NTP_to_Arduino, Send_NTP_to_Arduino);
  timer.restartTimer(TimerNTP);
#if defined VERBOSE
  Serial.println(F(">NTP client and timer started"));
#endif
#endif
}

//--------------------------------------------------------------------
// LOOP
//--------------------------------------------------------------------
void loop() {
//  if (restartFlag) ESP.restart();
//
//  // execute the timers
//  timer.run();
//
//  //-------------------------------------------------------
//  // RECUPERER LES CARACTERES ARRIVANT SUR LA LIAISON SERIE
//  //-------------------------------------------------------
//  while (Serial.available()) {
//    // get the new byte:
//    char inChar = (char)Serial.read();
//    // add it to the inputString:
//    inputString += inChar;
//    // if the incoming character is a newline, set a flag
//    // so the main loop can do something about it:
//    if (inChar == '\n') {
//      stringComplete = true;
//    }
//  }
//
//  if (WiFi.status() != WL_CONNECTED) {
//    //---------------------------------------------------
//    // SI LA CONNECTION EST PERDUE, TENTER DE LA RETABLIR
//    //---------------------------------------------------
//    Serial.println(F(">WiFi not connected !"));
//    WiFiConnect();
//
//  } else {
//    //--------------------------------------------------
//    // UPDATE DOMOTICZ the string when a newline arrives:
//    //--------------------------------------------------
//    if (stringComplete) {
//      parseString(inputString);
//    }
//  }
//
//  if (stringComplete) {
//    // clear the string:
//    inputString = "";
//    stringComplete = false;
//  }
//
//  //--------------------------
//  // EXECUTER LE SERVEUR WEB
//  //--------------------------
//  server.handleClient();
//
  //-------------------------
  // RECUPERER LE TEMPS NTP
  //-------------------------
#if defined NTP
  timeClient.update();
  TimeNTP = timeClient.getFormattedTime();
#endif
}
