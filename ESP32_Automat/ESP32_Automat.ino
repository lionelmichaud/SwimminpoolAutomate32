//
// VERSIONS HISTORY
//
// VERSION 2.3.2 - Compatible de AsyncTCP v1.0.0 et ESP32 v1.0.0
//  Inversion de la logique du swicth Maual / Automatique
//
// VERSION 2.3.1 - Compatible de AsyncTCP v1.0.0 et ESP32 v1.0.0
//  Inversion de l'affichage OLED (collé dans le boitier à l'envers) dans le fichier Config JSON
//  Permutation des numéros de device par défaut des sondes de température 0, 1 et 2
//  Ajout des print de température intérieure
//  Ajout de 2 commandes HTTP de permutation des numéros de device des sondes de température (0 <=> 1 et 0 <=> 2)
//  Correction de la gestion des offset de temp sonde par page Web
//
// VERSION 2.3.0 - Compatible de AsyncTCP v1.0.0 et ESP32 v1.0.0
//  Introduction de variantes du nb de pages OLED (2 ou 4): #define DEBUG_OLED
//  Inhibition possible des transmissions à Domoticz: #define DOMOTICZ_TX
//  Mesure de la température interne sur 3ième capteure 1wire Dallas
//  Température affichée avec 1 digit
//
// VERSION 2.2.0 - Compatible de AsyncTCP v1.0.0 et ESP32 v1.0.0
//  Introduction de variantes en fonction de la taille du display OLED: #define OLED_096
//
// VERSION 2.1.1 - Compatible de AsyncTCP v1.0.0 et ESP32 v1.0.0
//  Finetuning
//
// VERSION 2.1.0
//  Nouvelle page Web à onglet avec dernière version de Bootstrap 4.1.3
//
// VERSION 2.0.0
//  Version 1.0.0 + Multi Core
//
// VERSION 1.0.0
//  Version complète et testée - Mono Core
//
// VERSION 0.6.0
//  Passage au serveur WEB asynchrone
//  Amélioration de la gestion des mesures de température abérantes
//
// VERSION 0.3.0
//  Paramétrage inversion écran OLED #define Configuration.flipOLED
//  Lecture des paramètres réseaux Wi-Fi en fichier SPIFF config JSON (SSID / Password)
//
// VERSION 0.2.0
//  Première version complète
//
// VERSION 0.1.0
//  Création
//
//-------------------------------------------------
//   DEFINE
//-------------------------------------------------
// VERSION NUMBER
#define SOFTWARE "ESP32_POOL"
#define VERSION "2.3.2"

#define USB_OUTPUT
#define ECHO    // Echo toutes les commande reçues de l'Arduino vers l'Arduino après décodage
#define PREFERENCES_OUTPUT
//#define PREFERENCES_RESET
#define CLOSURE_TEMPO
#define NTP_OFFSET 3600 // 3600 = 1h en hiver ; 7200 = 2h en été
#define NTP_PERIOD 30000 // milisecondes
#define TEMPOFFSETINCREMENT 0.25
#define DOMOTICZ_TX // transmettre les valeurs à Domoticz
//#define OLED_096 // 0.96 else 1.30 inch
//#define DEBUG_OLED // 4 pages affichées (2 de + qu'en mode run)

// USB serial line bitrate
#define USBSERIAL_BITRATE 115200

#define TEMPERATURE_PRECISION 11

// SIMPLE DEBUG OPTIONS
// Disable all debug ? Good to release builds (production)
// as nothing of SerialDebug is compiled, zero overhead :-)
// For it just uncomment the DEBUG_DISABLED
//#define DEBUG_DISABLED true

// Disable SerialDebug debugger ? No more commands and features as functions and globals
// Uncomment this to disable it
//#define DEBUG_DISABLE_DEBUGGER true

// Debug TAG ?
// Usefull with debug any modules
// For it, each module must have a TAG variable:
//       const char* TAG = "...";
// Uncomment this to enable it
//#define DEBUG_USE_TAG true

// Define the initial debug level here (uncomment to do it)
#define DEBUG_INITIAL_LEVEL DEBUG_LEVEL_VERBOSE

// Force debug messages to can use flash ) ?
// Disable native Serial.printf (if have)
// Good for low memory, due use flash, but more slow and not use macros
//#define DEBUG_USE_FLASH_F true


//-------------------------------------------------
//   INCLUSION
//-------------------------------------------------
#include <SerialDebug.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "NTPClient.h"
#include "images.h"
#include "OLEDDisplayUi.h"
#include <SimpleTimer.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Update.h>
#include <DallasTemperature.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier

#if defined OLED_096
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#else
// or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
#include "SH1106Wire.h"
#endif

// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"
// Include custom images

//------------------------------------------------
// --- TASKS HANDLER DECLARATIONS (DUAL-CORE)  ---
//------------------------------------------------
TaskHandle_t AutomatTask;

//-------------------------------------------------
// --- Déclaration des constantes globales ---
//-------------------------------------------------
const char *ConfigFilename = "/config.json";
// Compute the required size: 6 x réseaux Wi-Fi max possible
const int JSONBufferConfigCapacity = JSON_ARRAY_SIZE(5) + 5 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(14) + 710;
// paramètres Wi-Fi - station
const char* Local_Name    = "esp32_pool";
// paramètres Wi-Fi - Access Point
const char* Automat_ssid  = "esp32_pool";
// timer variables
//  Generally, you should use "unsigned long" for variables that hold time
//  The value will quickly become too large for an int to store
const unsigned long intervalLED  =    500; // interval at which to blink the Red LED (milliseconds)
const unsigned long intervalUSB  =  60010; // interval at which to send data over USB (milliseconds)
const unsigned long delaySamplingTemp = 750 / (1 << (12 - TEMPERATURE_PRECISION)); // delay to sample the temperatures (milliseconds)
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
//const int pIntTempR =     A3; // the number of the IntTempR pin
// set PWM channels parameters
const int cTempLEDblue  = 0;
const int cTempLEDgreen = 1;
const int cTempLEDred   = 2;
const int LEDfreq = 5000; // Hz
const int LEDres  = 8; // bits => 256 levels

//-------------------------------------------------
// --- Déclaration de types ---
//-------------------------------------------------
struct idx_T { // IDX of Domoticz devices
  int idx_waterTemp = 48;
  int idx_airTemp   = 49;
  int idx_automate  = 50;
  int idx_posVolet  = 51;
};
// configuration du logiciel
struct domoticz_T {   // configuration Domoticz
  String  host = "192.168.1.23"; // "192.168.1.23";
  int     port = 8084;
  idx_T   idxs;         // list of Domoticz idx
};
struct WiFiNetwok_T {
  String ssid;
  String password;
};
struct Configuration_T {
  int             RedLEDtemp       = 20;
  int             GreenLEDtemp     = 27;
  bool            flipOLED         = false;
  unsigned long   intervalTemp     =   5000; // interval at which to sample the temperatures (milliseconds)
  unsigned long   timeoutOpenClose = 145000; // max duration of the opening or closure in mili-seconds (2minutes 25sec)
  unsigned long   intervalWiFi     =  60000; // interval at which to send data over WiFi (milliseconds)
  domoticz_T      domoticz;                  // Domoticz parameters
  String          automat_pwd      = "Levsmsa2";
  int             nbWiFiNetworks   = -1;
  WiFiNetwok_T*   WiFiNetworks;              // list of Wi-Fi networks
};
// Automat 1 : Mode de fonctionnement MANUAL || AUTOMATIC
const int MANUAL     = 0;
const int AUTOMATIC  = 1;
const int UNDEF_MODE = 2;
struct Automat_Mode_T {
  int ModeState = MANUAL;
  int prevModeState = MANUAL;
  boolean ErrorMode = false;
};
// Automat 2 : Commande Volet roulant CLOSE_CMD_ACTIVATED || OPEN_CMD_ACTIVATED || UNDEF_CMD
const int CLOSE_CMD_ACTIVATED = 0;
const int OPEN_CMD_ACTIVATED  = 1;
const int UNDEF_CMD = 2;
struct Automat_Cmd_T {
  int CommandState     = UNDEF_CMD;
  int prevCommandState = UNDEF_CMD;
  boolean ErrorCmd  = false;
};
// Etat de l'automate et de la piscine
struct PoolState_T {
  float AirTemp       = -256.0;
  float WaterTemp     = -256.0;
  float InternalTemp  = -256.0;
  boolean ErrorConfig = false;
  boolean ErrorTempSensorInit   = false; // erreur à l'initialisation de l'un des capteurs one wire
  boolean ErrorTempSensorInit0  = false; // erreur à l'initialisation du capteur one wire
  boolean ErrorTempSensorInit1  = false; // erreur à l'initialisation du capteur one wire
  boolean ErrorTempSensorInit2  = false; // erreur à l'initialisation du capteur one wire
  boolean ErrorTempAir          = false; // température air mesurée invalide
  boolean ErrorTempWater        = false; // température air mesurée invalide
};

//-------------------------------------------------
// --- Déclaration des variables globales ---
//-------------------------------------------------
// configuration du logiciel
Configuration_T Configuration;

// WEB variables
// -------------
// client NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", NTP_OFFSET, NTP_PERIOD);
// serveur WEB
//WebServer server(80);
AsyncWebServer server(80);
AsyncEventSource events("/events");
// client HTTP
HTTPClient http;

// OLED display variables
// ----------------------
//   Initialize the OLED display using Wire library
//   display(@, SDA, SCL)
#if defined OLED_096
SSD1306Wire  display(0x3c, pSDA, pSCL);
#else
SH1106Wire display(0x3c, pSDA, pSCL);
OLEDDisplayUi ui(&display);
#endif
int Ypos = 0;

// the timer object
// ----------------
SimpleTimer timer;
int TimerColdID;
int TimerWiFi;
int TimerTemp;
unsigned long prevMillis =  0; // (milliseconds)
unsigned long currentMillis =  0; // (milliseconds)

// Wi-Fi variables
// ---------------
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
String the_password = "";        // a string to hold the password
boolean stringComplete = false;  // whether the string is complete
boolean restartFlag = false;     // whether to restart the module (http command

// NTP variables
// -------------
String TimeNTP = "";
String DateNTP = "";

// Temperature sensor devices variables
// ------------------------------------
// Data wire is plugged into pin 7 on the Arduino
//int WaterTempDeviceID()    = 2; // #define WaterTempDeviceID()    1 // swap with device 1 if temperature does not correspond
//int AirTempDeviceID()      = 0; // #define AirTempDeviceID()      2 // swap with device 0 if temperature does not correspond
//int InternalTempDeviceID() = 1; // #define InternalTempDeviceID() 0 // swap with device 0 if temperature does not correspond

// Automat 1 : Mode de fonctionnement MANUAL || AUTOMATIC
// ---------
Automat_Mode_T Automat_Mode;

// Automat 2 : Commande Volet roulant CLOSE_CMD_ACTIVATED || OPEN_CMD_ACTIVATED || UNDEF_CMD
// ---------
Automat_Cmd_T Automat_Cmd;

// Etat de l'automate et de la piscine
// -----------------------------------
PoolState_T PoolState;

// Variables
// ---------
int AutoLED =    LOW;
int AutoSwitch = LOW; // Automatic = OUVERT/HIGH || Manual = FERME/LOW
int AutoSwitchState = MANUAL;
int TempLEDred =   0;
int TempLEDgreen = 0;
int TempLEDblue =  0;
int Relay1 = HIGH; // Relais au repos (3.3V présent en entrée) => La clé manuelle a le controle : automatisme débrayé
int Relay2 = HIGH; // Relais au repos (3.3V présent en entrée) => Volet Fermé
int PeriodOfLowAirTemp = 0; // compte le nombre de périodes 'periodColdTimer' pendant lesquelles la temp à été vue basse
float Temp =       0.0;

String inputString = "";         // a string to hold incoming data
String url = "";
boolean UnexpectedStringReceived = false; // string receieved from Arduino is not understood
String UnexpectedString = "";

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(pTempDS18B20);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature DallasSensors(&oneWire);
uint8_t DallasDeviceCount;

// arrays to hold device addresses
DeviceAddress Device0_Thermometer, Device1_Thermometer, Device2_Thermometer;

// préférences de réglage de l'application
Preferences preferences;

//--------------------------------------------------
// functions prototypes
//--------------------------------------------------
void InitializeIO();
boolean StartWiFiSoftAP(Configuration_T Config);
boolean ConnectToWiFi(Configuration_T Config);
void initializeOLED(Configuration_T Config);
void InitTemperatureSensors(Configuration_T Config);
void DisplayOneMoreLine(String line, OLEDDISPLAY_TEXT_ALIGNMENT textAlignment);
void DisplayAlert(String AlertText);
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void drawPageSoftwareInfo(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawPageWiFi_AP_Info(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawPageWiFi_ST_Info(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawDeviceInfoTemperatures(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void drawDeviceInfoStatus(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y);
void StartWEBserver ();
void MeasurePeriodOfCold();
void parseString(String receivedString);
void SetWaterTempOffset(float Offset);
void SetAirTempOffset(float Offset);
void SetSummerHour(bool SummerHour);
void SetSeuil(float Seuil);
void SetHysteresis(float Hysteresis);
bool SummerHour();
int NbPeriodCold();
float Seuil();
float Hysteresis();
float AirTempOffset();
float WaterTempOffset();
void ResetPreferences();
void DumpPreferences();
void print1wireAddress(DeviceAddress deviceAddress);
void print1wireTemperature(DeviceAddress deviceAddress);
void DisplayWaterTemperatureOnLED (int WaterTemp, Configuration_T Config);
void SampleTemperatures();
String String1wireAddress(DeviceAddress deviceAddress);
boolean Start_WiFi_IDE_OTA();
void SendDataOverUSB();
void BlinkRedAutoLED();
boolean SwitchRelayAutoManu (int State);
boolean SwitchRelayOpenCloseCover (int State);
void AutomatRun(Configuration_T Config, Automat_Mode_T& theAutomatMode, Automat_Cmd_T& theAutomatCmd, int theSwitchState);
void SendDataToDomoticz ();
boolean ReadConfig(const char *filename, Configuration_T& Config);
void AutomatTaskCode( void * pvParameters );

bool toggle (bool p) {
  return (p ? false : true);
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
#if defined DEBUG_OLED
FrameCallback frames[] = { drawPageSoftwareInfo,
                           drawPageWiFi_AP_Info, 
                           drawPageWiFi_ST_Info,
                           drawDeviceInfoTemperatures,
                           //drawDeviceInfoStatus
                         };
// how many frames are there?
int frameCount = 4;
#else
FrameCallback frames[] = { //drawPageSoftwareInfo,
                           //drawPageWiFi_AP_Info, 
                           drawPageWiFi_ST_Info,
                           drawDeviceInfoTemperatures,
                           //drawDeviceInfoStatus
                         };
// how many frames are there?
int frameCount = 2;
#endif

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

// Include SerialDebug
#include "SerialDebug.h" //https://github.com/JoaoLopesF/SerialDebug


//--------------------------------------------------------------------
// SETUP
//--------------------------------------------------------------------
void setup() {
  //-------------------------------
  // initialize serial
  //-------------------------------
  Serial.begin(USBSERIAL_BITRATE);
  delay(10);

  //------------------------------------------
  // INITIALIZE SPIFF LIBRARY
  //------------------------------------------
  while (!SPIFFS.begin()) {
    printlnA("Failed SPIFF initialization");
    DisplayAlert("Failed to initialize SPIFF library !");
  }

  //------------------------------------------
  // READ JSON CONFIGURATION FILE IN SPIFF
  //------------------------------------------
  // Should load default config if run for the first time
  printlnA("Loading configuration file in SPIFF...");
  PoolState.ErrorConfig = !ReadConfig(ConfigFilename, Configuration);

  // reserve n bytes for the Strings: to avoid dynamic realocations
  inputString.reserve(200);

  printlnA("");
  printlnA("--------------------------");
  printlnA("  ESP01 is booting ");
  printlnA("--------------------------");
  printA("  VERSION = "); printlnA(VERSION);
  printlnA("  I was compiled " __DATE__ );
  printlnA("--------------------------");

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
  // initialize IO ports
  //-------------------------------
  InitializeIO();

  //-------------------------------
  // INITIALIZE 1WIRE BUS and OLED
  //-------------------------------
  initializeOLED(Configuration);
  DisplayOneMoreLine("ESP32 booting...", TEXT_ALIGN_CENTER);
  DisplayOneMoreLine("VERSION : " + String(VERSION), TEXT_ALIGN_CENTER);
  DisplayOneMoreLine("compiled " + String(__DATE__), TEXT_ALIGN_CENTER);

  Serial.setDebugOutput(false);

  //--------------------------------------------------------------------
  // INITIALISATION DES TIMERS
  //--------------------------------------------------------------------
  // initialize timer call-backs
  printA("Initialisation timer LED = ");
  printlnA(millis());
  timer.setInterval(intervalLED, BlinkRedAutoLED);

  printA("Initialisation timer USB = ");
  printlnA(millis());
  timer.setInterval(intervalUSB, SendDataOverUSB);

#if defined CLOSURE_TEMPO
  printlnA("Initialisation timer période de refroidissement avant fermeture");
  TimerColdID = timer.setInterval(periodColdTimer, MeasurePeriodOfCold); // créer le timer
  timer.disable(TimerColdID); // désactiver le timer en attendant d'en avoir besoin
#endif

  //--------------------------------------------------------------------
  // INITIALISATION DES CAPTEURS DE TEMPERATURE
  //--------------------------------------------------------------------
  InitTemperatureSensors(Configuration);

  //--------------------------------------------------------------------------------------------------
  // Set WiFi to station mode + Access Point and disconnect from an AP if it was previously connected
  //--------------------------------------------------------------------------------------------------
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  delay(1000);

  //-----------------------------------
  // INITIALIZE SOFT WI-FI ACCESS POINT
  //-----------------------------------
  StartWiFiSoftAP(Configuration);
  delay(1000);

  //-----------------------------------------------
  // CONNECTER AU WI-FI ET DEMARRER LE SERVEUR WEB
  //-----------------------------------------------
  if (!ConnectToWiFi(Configuration)) return;
  delay(1000);

  //-------------------------
  // DEMARRER LE CLIENT NTP
  //-------------------------
  timeClient.begin();
  // décalage horaire et reculer d'une heure en heure d'hiver
  if (SummerHour()) timeClient.setTimeOffset(7200);
  else timeClient.setTimeOffset(3600);
  delay(1000);

  //-------------------------------------------
  // Timer transmission Wi-Fi vers Domoticz
  //-------------------------------------------
  printA("Initialisation timer Wi-Fi = ");
  printlnA(millis());
  TimerWiFi = timer.setInterval(Configuration.intervalWiFi, SendDataToDomoticz);
  //timer.restartTimer(TimerWiFi);

  //-----------------------------------------------------
  // --- Création des tâches concurrentes (dual-core) ---
  //-----------------------------------------------------
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    AutomatTaskCode,   /* Task function. */
    "AutomatTask",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &AutomatTask,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  printlnA("Automat task created on core 0...");

#ifndef DEBUG_DISABLE_DEBUGGER

  // Add Functions and global variables to SerialDebug

  // Add functions that can called from SerialDebug

  //debugAddFunctionVoid(F("function"), &function); // Example for function without args
  //debugAddFunctionStr(F("function"), &function); // Example for function with one String arg
  //debugAddFunctionInt(F("function"), &function); // Example for function with one int arg

  // Add global variables that can showed/changed from SerialDebug
  // Note: Only global, if pass local for SerialDebug, can be dangerous
  debugAddGlobalInt("TimerColdID", &TimerColdID);
  debugAddGlobalInt(F("TimerWiFi"), &TimerWiFi);
  debugAddGlobalInt(F("TimerTemp"), &TimerTemp);
  debugAddGlobalULong(F("prevMillis"), &prevMillis);
  debugAddGlobalULong(F("currentMillis"), &currentMillis);
  debugAddGlobalInt(F("selectedWiFi"), &selectedWiFi);
  debugAddGlobalInt(F("bestRSSI"), &bestRSSI);
  debugAddGlobalInt(F("theRSSI"), &theRSSI);
  debugAddGlobalString(F("the_IP_String"), &the_IP_String);
  debugAddGlobalString(F("the_AP_IP_String"), &the_AP_IP_String);
  debugAddGlobalString(F("the_MAC_String"), &the_MAC_String);
  debugAddGlobalString(F("the_SSID"), &the_SSID);
  debugAddGlobalString(F("the_password"), &the_password);
  debugAddGlobalBoolean(F("stringComplete"), &stringComplete);
  debugAddGlobalBoolean(F("restartFlag"), &restartFlag);
  debugAddGlobalString(F("TimeNTP"), &TimeNTP);
  debugAddGlobalString(F("DateNTP"), &DateNTP);
  debugAddGlobalInt(F("Ypos"), &Ypos);
  debugAddGlobalInt(F("AutoLED"), &AutoLED);
  debugAddGlobalInt(F("AutoSwitch"), &AutoSwitch);
  debugAddGlobalInt(F("AutoSwitchState"), &AutoSwitchState);
  debugAddGlobalInt(F("TempLEDred"), &TempLEDred);
  debugAddGlobalInt(F("TempLEDgreen"), &TempLEDgreen);
  debugAddGlobalInt(F("TempLEDblue"), &TempLEDblue);
  debugAddGlobalInt(F("Relay1"), &Relay1);
  debugAddGlobalInt(F("Relay2"), &Relay2);
  debugAddGlobalInt(F("PeriodOfLowAirTemp"), &PeriodOfLowAirTemp);
  debugAddGlobalFloat(F("Temp"), &Temp);
  debugAddGlobalString(F("inputString"), &inputString);
  debugAddGlobalString(F("url"), &url);
  debugAddGlobalBoolean(F("UnexpectedStringReceived"), &UnexpectedStringReceived);
  debugAddGlobalString(F("UnexpectedString"), &UnexpectedString);
  debugAddGlobalUInt8_t(F("DallasDeviceCount"), &DallasDeviceCount);
  debugAddGlobalInt(F("frameCount"), &frameCount);
  debugAddGlobalInt(F("overlaysCount"), &overlaysCount);

#endif // DEBUG_DISABLE_DEBUGGER

}

//--------------------------------------------------------------------
// LOOP (EXECUTES ON CORE 1 BY DEFAULT)
//--------------------------------------------------------------------
void loop() {

  // SerialDebug handle
  // Notes: if in inactive mode (until receive anything from serial),
  // it show only messages of always or errors level type
  // And the overhead during inactive mode is very low
  // Only if not DEBUG_DISABLED
  debugHandle();

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
    // RECUPERER LES CARACTERES ARRIVANT SUR LA LIAISON SERIE
    //-------------------------------------------------------
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
      // printV(inChar);
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
      printlnE(" > WiFi not connected !");
      ConnectToWiFi(Configuration);

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

//--------------------------------------------------------------------
// TASK AutomatTaskCode (EXECUTES ON CORE 0)
//--------------------------------------------------------------------
void AutomatTaskCode( void * pvParameters ) {
  while (true) {
    //-------------------------------------------------------
    // ACQUISITIONS OF INPUTS
    //-------------------------------------------------------
    // read the value from the Mode switch: Manual = FERME/LOW, Automatic = OUVERT/HIGH
    AutoSwitch = digitalRead(pAutoSwitch);
    if (AutoSwitch == HIGH)
      AutoSwitchState = AUTOMATIC;
    else
      AutoSwitchState = MANUAL;

    //-------------------------------------------------------
    // SET OUTPUTS
    //-------------------------------------------------------
    // set the Water temp LED color as a function of Water temperature
    DisplayWaterTemperatureOnLED (PoolState.WaterTemp, Configuration);

    //-------------------------------------------------------
    //  EXECUTE STATE MACHINES
    //-------------------------------------------------------
    AutomatRun(Configuration, Automat_Mode, Automat_Cmd, AutoSwitchState);

    delay(500);
  }
}
