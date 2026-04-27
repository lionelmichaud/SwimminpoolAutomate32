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
    printlnA("Failed to open config file !");
    DisplayAlert("Failed to open config file");
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, configFile);

  if (error) {
    printlnA("Failed to read JSON config file, using default configuration");
    DisplayAlert("Failed to read config file");
    return false;
  }
  serializeJsonPretty(doc, Serial);

  // get RVB LED parameters
  Config.RedLEDtemp   = doc["temperature LED rouge"] | 20;
  Config.GreenLEDtemp = doc["temperature LED verte"] | 27;

  // get OLED parameters
  Config.flipOLED = doc["flip OLED display"].as<bool>();

  // get timers values
  Config.intervalTemp     = 1000 * (doc["temperature sampling period (s)"] | 5);
  Config.timeoutOpenClose = 1000 * (doc["cover open/close duration (s)"] | 145);
  Config.intervalWiFi     = 1000 * (doc["wi-fi transmission period (s)"] |  60);

  // get Domoticz parameters
  Config.domoticz.host = doc["domoticz IP"] | "192.168.1.23";
  Config.domoticz.port = doc["domoticz port"] | 8084;
  Config.domoticz.idxs.idx_waterTemp = doc["idx temperature eau"] | 48;
  Config.domoticz.idxs.idx_airTemp   = doc["idx temperature air"] | 49;
  Config.domoticz.idxs.idx_automate  = doc["idx automate mode"]   | 50;
  Config.domoticz.idxs.idx_posVolet  = doc["idx position volet"]  | 51;

  // get Wi-Fi access point parameters
  Config.automat_pwd = doc["access point password"] | "Levsmsa2";

  //  Serial.println(); Serial.print("flipOLED : "); Serial.println(Config.flipOLED);
  //  Serial.println("Temporisations : "); Serial.println(Config.intervalTemp); Serial.println(Config.timeoutOpenClose); Serial.println(Config.intervalWiFi);
  //  Serial.println("IDX : "); Serial.println(Config.domoticz.idxs.idx_waterTemp); Serial.println(Config.domoticz.idxs.idx_airTemp); Serial.println(Config.domoticz.idxs.idx_automate);
  //  Serial.println("AP password : "); Serial.println(Config.automat_pwd);
  printlnA("Networks configurations: ");
  JsonArray NetArray = doc["networks"];

  Config.nbWiFiNetworks = NetArray.size();
  Config.WiFiNetworks = new WiFiNetwok_T[Config.nbWiFiNetworks];
  int i = 0;

  for (JsonObject elem : NetArray) {
    //    const char* ssid = elem["ssid"]; // "Mon BWi-Fi"
    //    const char* password = elem["password"]; // "louannetvanessasontmessourisadorees"
    Config.WiFiNetworks[i].ssid = elem["ssid"].as<String>();
    Config.WiFiNetworks[i].password = elem["password"].as<String>();
    printA("  - SSID: "); printA(Config.WiFiNetworks[i].ssid);
    printA("  PASSWORD: "); printlnA(Config.WiFiNetworks[i].password);
    i++;
  }
  // Close the file (File's destructor doesn't close the file)
  configFile.close();

  return true;
}
