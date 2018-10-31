//---------------------------------------------
// LIST DIRECTORY ON A FILE SYSTEM
//---------------------------------------------
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

//---------------------------------------------
// READ A JSON CONFIG FILE ON SPIFFS
//---------------------------------------------
boolean ReadConfig(const char *filename, Configuration_T& Config) {
  // List SPIFFS directory
  listDir(SPIFFS, "/", 0);

  // Open file for reading
  File configFile = SPIFFS.open(filename, "r");
  if (!configFile) {
    Serial.println(F("Failed to open config file !"));
    DisplayAlert("Failed to open config file");
    return false;
  }

  // Allocate the memory pool on the stack.
  // Don't forget to change the capacity to match your JSON document.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonBuffer<JSONBufferConfigCapacity> jsonBuffer;

  // Parse the root object
  JsonObject& root = jsonBuffer.parseObject(configFile);
  JsonObject& root_ = root;

  // vérifier le bon décodage
  if (!root.success())
  {
    Serial.println(F("Failed to read JSON config file, using default configuration"));
    DisplayAlert("Failed to read config file");
    return false;
  }
#if defined VERBOSE
  root_.prettyPrintTo(Serial);
#endif

  // get OLED parameters
  Config.flipOLED = root["flip OLED display"].as<bool>();

  // get timers values
  Config.intervalTemp     = 1000 * (root["temperature sampling period (s)"] | 5);
  Config.timeoutOpenClose = 1000 * (root["cover open/close duration (s)"] | 145);
  Config.intervalWiFi     = 1000 * (root["wi-fi transmission period (s)"] |  60);

  // get Domoticz parameters
  Config.domoticz.host = root["domoticz IP"] | "192.168.1.23";
  Config.domoticz.port = root["domoticz port"] | 8084;
  Config.domoticz.idxs.idx_waterTemp = root["idx temperature eau"] | 48;
  Config.domoticz.idxs.idx_airTemp   = root["idx temperature air"] | 49;
  Config.domoticz.idxs.idx_automate  = root["idx automate mode"]   | 50;
  Config.domoticz.idxs.idx_posVolet  = root["idx position volet"]  | 51;

  // get Wi-Fi access point parameters
  Config.automat_pwd = root["access point password"] | "Levsmsa2";

#if defined VERBOSE
//  Serial.println(); Serial.print("flipOLED : "); Serial.println(Config.flipOLED);
//  Serial.println("Temporisations : "); Serial.println(Config.intervalTemp); Serial.println(Config.timeoutOpenClose); Serial.println(Config.intervalWiFi);
//  Serial.println("IDX : "); Serial.println(Config.domoticz.idxs.idx_waterTemp); Serial.println(Config.domoticz.idxs.idx_airTemp); Serial.println(Config.domoticz.idxs.idx_automate);
//  Serial.println("AP password : "); Serial.println(Config.automat_pwd);
  Serial.println("Networks configurations: ");
#endif
  // Extract the wi-fi networks array
  JsonArray & NetArray = root_["networks"];

  Config.nbWiFiNetworks = NetArray.size();
  Config.WiFiNetworks = new WiFiNetwok_T[Config.nbWiFiNetworks];
  int i = 0;

  // Walk the JsonArray efficiently
  for (JsonObject& elem : NetArray) {
    JsonObject& network = elem;
    //    const char* ssid = elem["ssid"]; // "Mon BWi-Fi"
    //    const char* password = elem["password"]; // "louannetvanessasontmessourisadorees"
    Config.WiFiNetworks[i].ssid = elem["ssid"].as<String>();
    Config.WiFiNetworks[i].password = elem["password"].as<String>();
#if defined VERBOSE
    Serial.print("  - SSID: "); Serial.print(Config.WiFiNetworks[i].ssid);
    Serial.print("  PASSWORD: "); Serial.println(Config.WiFiNetworks[i].password);
#endif
    i++;
  }

  // Copy values from the JsonObject to the Config
  //config.port = root["port"] | 2731;
  //  strlcpy(config.ssid,           // <- destination
  //          root["ssid"] | " ",    // <- source
  //          sizeof(config.ssid));  // <- destination's capacity
  //  strlcpy(config.password,           // <- destination
  //          root["password"] | " ",    // <- source
  //          sizeof(config.password));  // <- destination's capacity

  // Close the file (File's destructor doesn't close the file)
  configFile.close();

  return true;
}
