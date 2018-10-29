void ReadConfig(const char *filename, Config_T &config) {
  // Open file for reading
  File file = SPIFFS.open(filename);

  // Allocate the memory pool on the stack.
  // Don't forget to change the capacity to match your JSON document.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonBuffer<ConfigCapacity> jsonBuffer;

  // Parse the root object
  // JsonObject &root = jsonBuffer.parseObject(file);
  JsonArray& root = jsonBuffer.parseArray(file);
  JsonArray& root_ = root;

  if (!root.success())
  {
    Serial.println(F("Failed to read file, using default configuration"));
    DisplayAlert("Failed to read file, using default configuration");
  }

  const char* root_0_ssid = root_[0]["ssid"]; // "Mon BWi-Fi"
  const char* root_0_password = root_[0]["password"]; // "louannetvanessasontmessourisadorees"

  const char* root_1_ssid = root_[1]["ssid"]; // "Mon Wi-Fi"
  const char* root_1_password = root_[1]["password"]; // "louannetvanessasontmessourisadorees"

  const char* root_2_ssid = root_[2]["ssid"]; // "Pool"
  const char* root_2_password = root_[2]["password"]; // "louannetvanessasontmessourisadorees"

  // Copy values from the JsonObject to the Config
  //config.port = root["port"] | 2731;
  //  strlcpy(config.ssid,           // <- destination
  //          root["ssid"] | " ",    // <- source
  //          sizeof(config.ssid));  // <- destination's capacity
  //  strlcpy(config.password,           // <- destination
  //          root["password"] | " ",    // <- source
  //          sizeof(config.password));  // <- destination's capacity

  // Close the file (File's destructor doesn't close the file)
  file.close();
}
