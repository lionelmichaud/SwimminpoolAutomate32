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
  // JsonObject &root = jsonBuffer.parseObject(file);
  JsonArray& root = jsonBuffer.parseArray(configFile);
  JsonArray& root_ = root;

  // vérifier le bon décodage
  if (!root.success())
  {
    Serial.println(F("Failed to read JSON config file, using default configuration"));
    DisplayAlert("Failed to read config file");
    return false;
  }

  // Walk the JsonArray efficiently
#if defined VERBOSE
  root_.prettyPrintTo(Serial);
  Serial.println(); Serial.println("Networks configurations: ");
#endif
  Config.NbWiFiNetworks = root_.size();
  Config.WiFiNetworks = new WiFiNetwok_T[Config.NbWiFiNetworks];
  int i = 0;

  for (JsonObject& elem : root_) {
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
