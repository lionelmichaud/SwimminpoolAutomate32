
// VERSIONS HISTORY

// VERSION 0.1.0
//  Création
//

// VERSION NUMBER
#define SOFTWARE "ESP32_Automat"
#define VERSION "0.1.0"

//#define DEBUG
#define VERBOSE // Infos de version et de connection WiFi
#define ECHO    // Echo toutes les commande reçues de l'Arduino vers l'Arduino après décodage
#define NTP_OFFSET 7200 // 3600 = 1h en hiver ; 7200 = 2h en été
#define NTP_PERIOD 30000 // milisecondes

// USB serial line bitrate
#define USBSERIAL_BITRATE 115200

// IDX of Domoticz devices
#define idx_waterTemp 48
#define idx_airTemp   49
#define idx_automate  50
#define idx_posVolet  51

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <SimpleTimer.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"
// Include custom images
#include "images.h"
#include "OLEDDisplayUi.h"


// client NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", NTP_OFFSET, NTP_PERIOD);

// paramètres Wi-Fi
const char* Local_Name    = "esp01_pool_13";
const char* Pool_ssid     = "Pool";
const char* Garage_ssid   = "Garage";
const char* MonWiFi_ssid  = "Mon Wi-Fi";
const char* MonBWiFi_ssid = "Mon BWi-Fi";
const char* password = "louannetvanessasontmessourisadorees"; // pour tous les réseaux Wi-Fi

const char* Automat_ssid  = "esp01_pool_13";
const char* Automat_pwd = "Levsmsa2";

const char* host = "192.168.1.23"; // "192.168.1.23";
const int   port = 8084;
const int   aDelay = 500; // délai entre lignes envoyées sur Serial
//HTTPClient http;

// OLED display
char OLED_buffer[20];
// Initialize the OLED display using Wire library
// display(@, SDA, SCL)
SSD1306Wire  display(0x3c, 4, 2);
// SH1106 display(0x3c, D3, D5);
OLEDDisplayUi ui     ( &display );

IPAddress the_IP;
IPAddress AP_IP;
byte mac[6];
int selectedWiFi = -1;
int bestRSSI = -600;
int theRSSI;
String the_IP_String = "unattributed";
String the_AP_IP_String = "unattributed";
String the_MAC_String = "";
String the_SSID = "";            // a string to hold the SSID
boolean stringComplete = false;  // whether the string is complete
boolean restartFlag = false;     // whether to restart the module (http command

boolean UnexpectedStringReceived = false; // string receieved from Arduino is not understood
String UnexpectedString = "";

boolean initAirTemp    = true;
boolean initWaterTemp  = true;

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
String DateNTP = "";
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
// constants won't change :
const unsigned long interval_NTP_to_Arduino  =  61000; // interval at which to send NTP time over USB (milliseconds)
// the timer object
SimpleTimer timer;

// --- Déclaration des variables globales ---
const int fontsize = 10;
int Ypos = 0;

void Print_NTP_Date_Time() {
  Serial.println(">Time = " + TimeNTP);
}
boolean StartWiFiSoftAP();
boolean ConnectToWiFi();

//--------------------------------------------------------------------
// SETUP
//--------------------------------------------------------------------
void setup() {
  // reserve n bytes for the Strings: to avoid dynamic realocations
  AutomateString.reserve(50);
  VoletString.reserve(50);
  SoftwareString.reserve(50);
  inputString.reserve(200);

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

  Serial.setDebugOutput(false);

  //--------------------------------------------------------------------------------------------------
  // Set WiFi to station mode + Access Point and disconnect from an AP if it was previously connected
  //--------------------------------------------------------------------------------------------------
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  delay(1000);

  //-----------------------------------
  // INITIALIZE SOFT WI-FI ACCESS POINT
  //-----------------------------------
  StartWiFiSoftAP();
  delay(1000);

  //-------------------------
  // DEMARRER LE SERVEUR WEB
  //-------------------------
  if (!ConnectToWiFi()) return;
  delay(aDelay);

  //-------------------------
  // DEMARRER LE CLIENT NTP
  //-------------------------
  timeClient.begin();
  int TimerNTP = timer.setInterval(interval_NTP_to_Arduino, Print_NTP_Date_Time);
  timer.restartTimer(TimerNTP);
#if defined VERBOSE
  Serial.println(F(">NTP client and timer started"));
#endif
  delay(2000);
}

//--------------------------------------------------------------------
// LOOP
//--------------------------------------------------------------------
void loop() {
  if (restartFlag) ESP.restart();

  //  // execute the timers
  timer.run();

  //-------------------------------------------------------
  // RECUPERER LES CARACTERES ARRIVANT SUR LA LIAISON SERIE
  //-------------------------------------------------------
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // Serial.print(inChar);
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    //---------------------------------------------------
    // SI LA CONNECTION EST PERDUE, TENTER DE LA RETABLIR
    //---------------------------------------------------
    Serial.println(F(">WiFi not connected !"));
    //WiFiConnect();

  } else {
    //--------------------------------------------------
    // UPDATE DOMOTICZ the string when a newline arrives:
    //--------------------------------------------------
    if (stringComplete) {
      //parseString(inputString);
    }
  }

  if (stringComplete) {
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  //  //--------------------------
  //  // EXECUTER LE SERVEUR WEB
  //  //--------------------------
  //  server.handleClient();
  //

  //-------------------------
  // RECUPERER LE TEMPS NTP
  //-------------------------
  timeClient.update();
  TimeNTP = timeClient.getFormattedTime();
  //  DateNTP = timeClient.getFormattedDate();
}
