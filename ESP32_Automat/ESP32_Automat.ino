
// VERSIONS HISTORY

// VERSION 0.1.0
//  Création
//

//-------------------------------------------------
//   DEFINE
//-------------------------------------------------
// VERSION NUMBER
#define SOFTWARE "ESP32_POOL"
#define VERSION "0.1.0"

#define DEBUG
#define USB_OUTPUT
#define VERBOSE // Infos de version et de connection WiFi
#define ECHO    // Echo toutes les commande reçues de l'Arduino vers l'Arduino après décodage
#define PREFERENCES_OUTPUT
//#define PREFERENCES_RESET
#define CLOSURE_TEMPO
#define NTP_OFFSET 7200 // 3600 = 1h en hiver ; 7200 = 2h en été
#define NTP_PERIOD 30000 // milisecondes
#define TEMPOFFSETINCREMENT 0.25

// USB serial line bitrate
#define USBSERIAL_BITRATE 115200

// IDX of Domoticz devices
#define idx_waterTemp 48
#define idx_airTemp   49
#define idx_automate  50
#define idx_posVolet  51

// Data wire is plugged into pin 7 on the Arduino
#define ONE_WIRE_BUS 7
#define ONE_WIRE_WATER_TEMP_DEVICE 0 // swap with device 1 if temperature does not correspond
#define ONE_WIRE_AIR_TEMP_DEVICE 1   // swap with device 0 if temperature does not correspond
#define TEMPERATURE_PRECISION 11

//-------------------------------------------------
//   INCLUSION
//-------------------------------------------------
#include <Preferences.h>
#include "NTPClient.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Update.h>
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
#include <DallasTemperature.h>

//-------------------------------------------------
// --- Déclaration des constantes globales ---
//-------------------------------------------------
const int   aDelay = 50; // délai entre lignes envoyées sur Serial
// paramètres Wi-Fi - station
const char* Local_Name    = "esp32_pool";
const char* Pool_ssid     = "Pool";
const char* Garage_ssid   = "Garage";
const char* MonWiFi_ssid  = "Mon Wi-Fi";
const char* MonBWiFi_ssid = "Mon BWi-Fi";
const char* password = "louannetvanessasontmessourisadorees"; // pour tous les réseaux Wi-Fi
// paramètres Wi-Fi - Access Point
const char* Automat_ssid  = "esp32_pool";
const char* Automat_pwd = "Levsmsa2";
// Domoticz
const char* host = "192.168.1.23"; // "192.168.1.23";
const int   port = 8084;
// timer variables
//  Generally, you should use "unsigned long" for variables that hold time
//  The value will quickly become too large for an int to store
const unsigned long intervalUSB  =  60010; // interval at which to send data over USB (milliseconds)
const unsigned long intervalWiFi =  15000; // interval at which to send data over WiFi (milliseconds)
const unsigned long intervalLED  =    500; // interval at which to blink the Red LED (milliseconds)
const unsigned long intervalTemp  =  5000; // interval at which to sample the temperatures (milliseconds)
const unsigned long delaySamplingTemp = 750 / (1 << (12 - TEMPERATURE_PRECISION)); // delay to sample the temperatures (milliseconds)
const unsigned long timeoutOpenClose  = 145000; // max duration of the opening or closure in mili-seconds (2minutes 25sec)
const unsigned long periodColdTimer   =  60000; // période de la vérification la baisse de température (1 minute)
// OLED constants
const int fontsize = 10;
// set GPIO pin numbers
//   Side 1
const int pSDA        =  22;  // the number of the SCL pin on I2C bus
const int pSCL        =  19;  // the number of the SDA pin on I2C bus
const int pAutoSwitch =  23;  // the number of the AutoSwitch pin
const int pAutoLED    =  18;  // the number of the AutoLED pin
const int pTempLEDblue =  5; // the number of the TempLEDblue pin
const int pDummy1     =  17;  // spare
const int pDummy2     =  16;  // spare
const int pTempLEDgreen = 4;  // the number of the TempLEDgreen pin
const int pDummy3     =   0;  // spare
const int pDummy4     =   2;  // spare
const int pTempLEDred =  15;  // the number of the TempLEDred pin
const int pTempDS18B20 = 13;  // the number of the TempDS18B20 pin
//   Side 2
const int pDummy5     =  34;  // spare
const int pDummy6     =  35;  // spare
const int pDummy7     =  32;  // spare
const int pDummy8     =  33;  // spare
const int pDummy9     =  25;  // spare
const int pDummy10    =  26;  // spare
const int pDummy11    =  27;  // spare
const int pRelay2     =  12;  // the number of the Relay2 pin
const int pRelay1     =  14;  // the number of the Relay1 pin
//const int pAirR =         A0; // the number of the AirR pin
//const int pWaterR =       A1; // the number of the WaterR pin
//const int pTrimDelta =    A2; // the number of the TrimDelta pin
//const int pIntTempR =     A3; // the number of the IntTempR pin
// set PWM channels parameters
const int cTempLEDblue  = 0;
const int cTempLEDgreen = 1;
const int cTempLEDred   = 2;
const int LEDfreq = 5000; // Hz
const int LEDres  = 8; // bits => 256 levels

//-------------------------------------------------
// --- Déclaration des variables globales ---
//-------------------------------------------------
// client NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", NTP_OFFSET, NTP_PERIOD);
// serveur WEB
WebServer server(80);
// client HTTP
HTTPClient http;
// OLED display
//   Initialize the OLED display using Wire library
//   display(@, SDA, SCL)
SSD1306Wire  display(0x3c, pSDA, pSCL);
// SH1106 display(0x3c, D3, D5);
OLEDDisplayUi ui(&display);
// the timer object
SimpleTimer timer;
int TimerColdID;
int TimerWiFi;
int TimerTemp;
unsigned long prevMillis =  0; // (milliseconds)
unsigned long currentMillis =  0; // (milliseconds)
// Wi-Fi variables
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
// NTP variables
String TimeNTP = "";
String DateNTP = "";
// OLED variables
int Ypos = 0;

// Automat 1
const int MANUAL    = 0;
const int AUTOMATIC = 1;
struct Automat_1_state_T {
  int ModeState = MANUAL;
  int prevModeState = MANUAL;
};
Automat_1_state_T Automat_1_state;

// Automat 2
const int CLOSE_CMD_ACTIVATED = 0;
const int OPEN_CMD_ACTIVATED  = 1;
const int UNDEF_CMD           = 2;
struct Automat_2_state_T {
  int CommandState     = UNDEF_CMD;
  int prevCommandState = UNDEF_CMD;
};
Automat_2_state_T Automat_2_state;

// Etat de l'automate et de la piscine
struct PoolState_T {
  float AirTemp       = 0.0;
  float WaterTemp     = 0.0;
  String AutomateString = "";
  String VoletString = "";
};
PoolState_T PoolState;

// Variables
int AutoLED =    LOW;
int AutoSwitch = HIGH; // Automatic = FERME/LOW, Manual = OUVERT/HIGH
int AutoSwitchState = MANUAL;
int TempLEDred =   0;
int TempLEDgreen = 0;
int TempLEDblue =  0;
int Relay1 = HIGH; // Relais au repos => La clé manuelle à la main
int Relay2 = HIGH; // Relais au repos => Fermé
int PeriodOfLowAirTemp = 0; // compte le nombre de périodes 'periodColdTimer' pendant lesquelles la temp à été vue basse
float Temp =       0.0;
boolean ErrorTemp = false;
boolean ErrorTemp0 = false;
boolean ErrorTemp1 = false;
boolean ErrorMode = false;
boolean ErrorCmd  = false;

String inputString = "";         // a string to hold incoming data
String url = "";
boolean UnexpectedStringReceived = false; // string receieved from Arduino is not understood
String UnexpectedString = "";

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature DallasSensors(&oneWire);
uint8_t DallasDeviceCount;

// arrays to hold device addresses
DeviceAddress Device0_Thermometer, Device1_Thermometer;

// préférences de réglage de l'application
Preferences preferences;

// WiFi transmission
int WiFiserialCycle = 1;

//--------------------------------------------------
// functions prototypes
//--------------------------------------------------
boolean StartWiFiSoftAP();
boolean ConnectToWiFi();
void initializeOLED();
void DisplayOneMoreLine(String line, OLEDDISPLAY_TEXT_ALIGNMENT textAlignment);
void DisplayAlert(String AlertText);
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void drawPageSoftwareInfo(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawPageWiFi_AP_Info(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawPageWiFi_ST_Info(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawDeviceInfoTemperatures(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawDeviceInfoStatus(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void StartWEBserver ();
void BlinkRedAutoLED ();
void MeasurePeriodOfCold();
void parseString(String receivedString);
void SetWaterTempOffset(float Offset);
void SetAirTempOffset(float Offset);
void SetSeuil(float Seuil);
void SetHysteresis(float Hysteresis);
int NbPeriodCold();
float Seuil();
float Hysteresis();
float AirTempOffset();
float WaterTempOffset();
void ResetPreferences();
void DumpPreferences();
void print1wireAddress(DeviceAddress deviceAddress);
void print1wireTemperature(DeviceAddress deviceAddress);
void DisplayWaterTemperatureOnLED (int WaterTemp);
void SampleTemperatures();
void InitTemperatureSensors();
String String1wireAddress(DeviceAddress deviceAddress);
boolean Start_WiFi_IDE_OTA();
void SendDataUSB();

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawPageSoftwareInfo, drawPageWiFi_AP_Info, drawPageWiFi_ST_Info,
                           drawDeviceInfoTemperatures,
                           //drawDeviceInfoStatus
                         };
// how many frames are there?
int frameCount = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

//--------------------------------------------------------------------
// SETUP
//--------------------------------------------------------------------
void setup() {
  //-------------------------------
  // initialize serial
  //-------------------------------
  Serial.begin(USBSERIAL_BITRATE);
  delay(10);

  // reserve n bytes for the Strings: to avoid dynamic realocations
  PoolState.AutomateString.reserve(50);
  PoolState.VoletString.reserve(50);
  inputString.reserve(200);

#if defined VERBOSE
  Serial.println("");
  Serial.println(F("--------------------------"));
  delay(aDelay);
  Serial.println(F("  ESP01 is booting "));
  delay(aDelay);
  Serial.println(F("--------------------------"));
  delay(aDelay);
  Serial.print(F("  VERSION = ")); Serial.println(VERSION);
  delay(aDelay);
  Serial.println(F("  I was compiled " __DATE__ ));
  delay(aDelay);
  Serial.println(F("--------------------------"));
  delay(aDelay);
#endif

  //------------------------------------------
  // reset / dump application preferences
  //------------------------------------------
  // Open Preferences with my-app namespace. Each application module, library, etc
  // has to use a namespace name to prevent key name collisions. We will open storage in
  // RW-mode (second parameter has to be false).
  // Note: Namespace name is limited to 15 chars.
  preferences.begin("app-pref", false);
#if defined PREFERENCES_RESET
  // initialize Preferences value
  ResetPreferences();
#endif
  DumpPreferences();

  //-------------------------------
  // initialize ports
  //-------------------------------
  // inputs
  //  pinMode(pIntTempR, INPUT);
  pinMode(pAutoSwitch, INPUT_PULLUP);
  // outputs
  pinMode(pAutoLED, OUTPUT);
  pinMode(pTempLEDred, OUTPUT);
  pinMode(pTempLEDgreen, OUTPUT);
  pinMode(pTempLEDblue, OUTPUT);
  pinMode(pRelay1, OUTPUT);
  pinMode(pRelay2, OUTPUT);

  // open relays
  digitalWrite(pRelay1, Relay1);
  digitalWrite(pRelay2, Relay2);

  // switch off Auto LED
  digitalWrite(pAutoLED, AutoLED);

  // configure LED PWM functionalitites
  ledcSetup(cTempLEDred,   LEDfreq, LEDres);
  ledcSetup(cTempLEDgreen, LEDfreq, LEDres);
  ledcSetup(cTempLEDblue,  LEDfreq, LEDres);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pTempLEDred,   cTempLEDred);
  ledcAttachPin(pTempLEDgreen, cTempLEDgreen);
  ledcAttachPin(pTempLEDblue,  cTempLEDblue);
  // switch off Water Temperature LED color between red (cold) and green (hot):
  //  analogWrite(pTempLEDred,   0);
  //  analogWrite(pTempLEDgreen, 0);
  //  analogWrite(pTempLEDblue,  0);
  ledcWrite(cTempLEDred, 0);
  ledcWrite(cTempLEDgreen, 0);
  ledcWrite(cTempLEDblue, 0);

  //-------------------------------
  // INITIALIZE 1WIRE BUS and OLED
  //-------------------------------
  initializeOLED();
  DisplayOneMoreLine("ESP32 booting...", TEXT_ALIGN_CENTER);
  DisplayOneMoreLine("VERSION : " + String(VERSION), TEXT_ALIGN_CENTER);
  DisplayOneMoreLine("compiled " + String(__DATE__), TEXT_ALIGN_CENTER);

  Serial.setDebugOutput(false);

  //--------------------------------------------------------------------
  // INITIALISATION DES TIMERS
  //--------------------------------------------------------------------
  // initialize timer call-backs
  Serial.print(F("Initialisation timer LED = "));
  Serial.println(millis());
  timer.setInterval(intervalLED, BlinkRedAutoLED);

#if defined USB_OUTPUT
  Serial.print(F("Initialisation timer USB = "));
  Serial.println(millis());
  timer.setInterval(intervalUSB, SendDataUSB);
#endif

#if defined CLOSURE_TEMPO
  Serial.println(F("Initialisation timer période de refroidissement avant fermeture"));
  TimerColdID = timer.setInterval(periodColdTimer, MeasurePeriodOfCold); // créer le timer
  timer.disable(TimerColdID); // désactiver le timer en attendant d'en avoir besoin
#endif

  //--------------------------------------------------------------------
  // INITIALISATION DES CAPTEURS DE TEMPERATURE
  //--------------------------------------------------------------------
  delay (1000); // retarder l'init des thermistance
  InitTemperatureSensors();

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

  //-----------------------------------------------
  // CONNECTER AU WI-FI ET DEMARRER LE SERVEUR WEB
  //-----------------------------------------------
  if (!ConnectToWiFi()) return;
  delay(1000);

  //-------------------------
  // DEMARRER LE CLIENT NTP
  //-------------------------
  timeClient.begin();
  //  int TimerNTP = timer.setInterval(interval_NTP_to_Arduino, Print_NTP_Date_Time);
  //  timer.restartTimer(TimerNTP);
  //#if defined VERBOSE
  //  Serial.println(F(" > NTP client and timer started"));
  //#endif
  delay(1000);
}

//--------------------------------------------------------------------
// LOOP
//--------------------------------------------------------------------
void loop() {
  if (restartFlag) {
    preferences.end();
    ESP.restart();
  }

  //-------------------------------------------------------
  //  Executer les timers
  //-------------------------------------------------------
  timer.run();

  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.

    //-------------------------------------------------------
    // ACQUISITIONS OF INPUTS
    //-------------------------------------------------------

    // read the value from the sensor: Internal Temperature
    //IntTempR = TemperatureFromTMP36 (pIntTempR) + TEMPOFFSETINT;

    // read the value from the Mode switch: Automatic = FERME/LOW, Manual = OUVERT/HIGH
    AutoSwitch = digitalRead(pAutoSwitch);
    if (AutoSwitch == LOW)
      AutoSwitchState = AUTOMATIC;
    else
      AutoSwitchState = MANUAL;

    //-------------------------------------------------------
    // SET OUTPUTS
    //-------------------------------------------------------
    // set the Water temp LED color as a function of Water temperature
    DisplayWaterTemperatureOnLED (PoolState.WaterTemp);

    //-------------------------------------------------------
    //  Stocker Temp interieure max dans EEPROM
    //-------------------------------------------------------
#if defined PREFERENCES_OUTPUT
    //    SetMaxIntTemp(IntTempR);
#endif

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
      //----------------------
      // ETEINDRE LA LED ROUGE
      //----------------------
      // digitalWrite(redLEDpin, LOW);

      //---------------------------------------------------
      // SI LA CONNECTION EST PERDUE, TENTER DE LA RETABLIR
      //---------------------------------------------------
      Serial.println(F(" > WiFi not connected !"));
      ConnectToWiFi();

    } else {
      //--------------------------------------------------
      // UPDATE DOMOTICZ the string when a newline arrives:
      //--------------------------------------------------
      if (stringComplete) {
        parseString(inputString);
      }
    }

    if (stringComplete) {
      // clear the string:
      inputString = "";
      stringComplete = false;
    }

    //--------------------------
    // EXECUTER LE SERVEUR WEB
    //--------------------------
    server.handleClient();

    //---------------------------------------------
    // SURVEILLER UNE DEMANDE DE TELECHARGEMENT IDE
    //---------------------------------------------
    ArduinoOTA.handle();

    //-------------------------
    // RECUPERER LE TEMPS NTP
    //-------------------------
    timeClient.update();
    TimeNTP = timeClient.getFormattedTime();
    DateNTP = timeClient.getFormattedDate();
  }
}

//***************************************************************
boolean LongPeriodOfLowAirTemp ()
{
  return (PeriodOfLowAirTemp >= NbPeriodCold());
}

//**************************************************************
//  Measure how long the air temp is lower than water temp
//***************************************************************
void MeasurePeriodOfCold()
{
#if defined DEBUG
  Serial.println("-------------------------------- -TIMER CALLBACK------------------------------ -");
  Serial.print(F("Millis = "));    Serial.println(currentMillis);
#endif
  if (PoolState.AirTemp < (PoolState.WaterTemp + Seuil() - Hysteresis()))
  {
    PeriodOfLowAirTemp = PeriodOfLowAirTemp + 1;
#if defined DEBUG
    Serial.print(F("Duration = "));    Serial.println(PeriodOfLowAirTemp);
#endif
  }
  else
    PeriodOfLowAirTemp = 0;
}

//********************************************
//  blink Red Auto LED when in AUTOMATIC Mode
//********************************************
void BlinkRedAutoLED ()
{
  if (ErrorTemp)
    // Allumer la LED rouge en fixe
    digitalWrite(pAutoLED, HIGH);
  else if (Automat_1_state.ModeState == AUTOMATIC) {
    // Faire clignoter la LED rouge
    AutoLED = !AutoLED;
    digitalWrite(pAutoLED, AutoLED);
  }
  else
    // Eteindre la LED rouge
    digitalWrite(pAutoLED, LOW);
}
