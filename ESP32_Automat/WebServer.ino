void ResetOffsetPreferences();

//--------------------------------------------------------------------
static String getContentType(const String& path) {
  if (path.endsWith(".html")) return "text/html";
  else if (path.endsWith(".htm")) return "text/html";
  else if (path.endsWith(".css")) return "text/css";
  else if (path.endsWith(".txt")) return "text/plain";
  else if (path.endsWith(".js")) return "application/javascript";
  else if (path.endsWith(".png")) return "image/png";
  else if (path.endsWith(".gif")) return "image/gif";
  else if (path.endsWith(".jpg")) return "image/jpeg";
  else if (path.endsWith(".ico")) return "image/x-icon";
  else if (path.endsWith(".svg")) return "image/svg+xml";
  else if (path.endsWith(".ttf")) return "application/x-font-ttf";
  else if (path.endsWith(".otf")) return "application/x-font-opentype";
  else if (path.endsWith(".woff")) return "application/font-woff";
  else if (path.endsWith(".woff2")) return "application/font-woff2";
  else if (path.endsWith(".eot")) return "application/vnd.ms-fontobject";
  else if (path.endsWith(".sfnt")) return "application/font-sfnt";
  else if (path.endsWith(".xml")) return "text/xml";
  else if (path.endsWith(".pdf")) return "application/pdf";
  else if (path.endsWith(".zip")) return "application/zip";
  else if (path.endsWith(".gz")) return "application/x-gzip";
  else if (path.endsWith(".appcache")) return "text/cache-manifest";
  return "application/octet-stream";
}

//--------------------------------------------------------------------
//bool handleFileRead(String path) { // send the SPIFFS file to the client (if it exists)
//  Serial.println("handleFileRead: " + path);
//  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
//  String contentType = getContentType(path);            // Get the MIME type
//  if (SPIFFS.exists(path)) {                            // If the file exists
//    File file = SPIFFS.open(path, "r");                 // Open it
//    size_t sent = server.streamFile(file, contentType); // And send it to the client
//    file.close();                                       // Then close the file again
//    return true;
//  }
//  Serial.println("\tFile Not Found");
//  return false;                                         // If the file doesn't exist, return false
//}

//--------------------------------------------------------------------
// Replaces placeholder in the HTML file with their value
//--------------------------------------------------------------------
String processor(const String& var) {
  printV(var + " = ");
  if (var == "Software") {
    String substitute = SOFTWARE;
    printlnV(substitute);
    return substitute;
  }
  else if (var == "Version") {
    String substitute = VERSION;
    printlnV(substitute);
    return substitute;
  }
  else if (var == "FLASHCHIP_SPEED") {
    String substitute = String(ESP.getFlashChipSpeed() / 1000000) + " MHz";
    printlnV(substitute);
    return substitute;
  }
  else if (var == "FLASHCHIP_MODE") {
    FlashMode_t ideMode = ESP.getFlashChipMode();
    String substitute = String((ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
    printlnV(substitute);
    return substitute;
  }
  else if (var == "AP_IP") {
    String substitute = the_AP_IP_String;
    printlnV(substitute);
    return substitute;
  }
  else if (var == "AUTOMAT_SSID") {
    String substitute = String(Automat_ssid);
    printlnV(substitute);
    return substitute;
  }
  else if (var == "AP_MAC") {
    String substitute = WiFi.softAPmacAddress();
    printlnV(substitute);
    return substitute;
  }
  else if (var == "ST_IP") {
    String substitute = the_IP_String;
    printlnV(substitute);
    return substitute;
  }
  else if (var == "LOCAL_NAME") {
    String substitute = String(Local_Name);
    printlnV(substitute);
    return substitute;
  }
  else if (var == "ST_SSID") {
    String substitute = the_SSID;
    printlnV(substitute);
    return substitute;
  }
  else if (var == "BEST_RSSI") {
    String substitute = String(bestRSSI);
    printlnV(substitute);
    return substitute;
  }
  else if (var == "ST_MAC") {
    String substitute = the_MAC_String;
    printlnV(substitute);
    return substitute;
  }
  else if (var == "DATE_NTP") {
    String substitute = DateNTP;
    printlnV(substitute);
    return substitute;
  }
  else if (var == "TIME_NTP") {
    String substitute = TimeNTP;
    printlnV(substitute);
    return substitute;
  }
  //---------------------------------------
  // DEBUT PARTIE SPECIFIQUE
  //---------------------------------------
  else if (var == "AIRTEMP") {
    String substitute = String(PoolState.AirTemp, 1);
    printlnV(substitute);
    return substitute;
  }
  else if (var == "ErrorTempAir") {
    String substitute = (PoolState.ErrorTempAir ? "(err)" : "");
    printlnV(substitute);
    return substitute;
  }
  else if (var == "EAUTEMP") {
    String substitute = String(PoolState.WaterTemp, 1);
    printlnV(substitute);
    return substitute;
  }
  else if (var == "ErrorTempEau") {
    String substitute = (PoolState.ErrorTempWater ? "(err)" : "");
    printlnV(substitute);
    return substitute;
  }
  else if (var == "CURRENT_MODE") {
    String substitute = CurrentModeString();
    printlnV(substitute);
    return substitute;
  }
  else if (var == "CURRENT_COVER_POSITION") {
    String substitute = CurrentCoverPositionString();
    printlnV(substitute);
    return substitute;
  }
  else if (var == "SEUIL_OUVRE") {
    String substitute = String(Seuil());
    printlnV(substitute);
    return substitute;
  }
  else if (var == "SEUIL_FERME") {
    String substitute = String(Seuil() - Hysteresis());
    printlnV(substitute);
    return substitute;
  }
  else if (var == "AIR_TEMP_OFFSET") {
    String substitute = String(AirTempOffset());
    printlnV(substitute);
    return substitute;
  }
  else if (var == "WATER_TEMP_OFFSET") {
    String substitute = String(WaterTempOffset());
    printlnV(substitute);
    return substitute;
  }
  else if (var == "AIR_ID") {
    String substitute = String(AirTempDeviceID());
    printlnV(substitute);
    return substitute;
  }
  else if (var == "EAU_ID") {
    String substitute = String(WaterTempDeviceID());
    printlnV(substitute);
    return substitute;
  }
  else if (var == "INT_ID") {
    String substitute = String(InternalTempDeviceID());
    printlnV(substitute);
    return substitute;
  }
  //---------------------------------------
  // FIN PARTIE SPECIFIQUE
  //---------------------------------------

  return String();
}


//--------------------------------------------------------------------
void handleTextInfo(AsyncWebServerRequest *request) {
  FlashMode_t ideMode = ESP.getFlashChipMode();
  //  uint32_t chipId = ESP.getFlashChipId();

  String message = "Logiciel ESP: " + String(SOFTWARE);
  message += " - Version: " + String(VERSION);
  //  message += "\nESP Flash size:  " + String(ESP.getFlashChipRealSize() / 1024) + " kByte";
  //  message += "\nESP Free sketch space: " + String(ESP.getFreeSketchSpace() / 1024) + " kByte";
  message += "\nESP Flash clock: " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz";
  message += "\nESP Flash mode:  " + String((ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  //  message += "\nESP Chip ID:     " + String(chipId);
  //  message += "\nESP Chip ID Flash size: " + getFlashChipSizeByChipId(chipId);
  message += "\n\nTime NTP:" + DateNTP + " / " + TimeNTP;
  message += "\n\nAccess point:";
  message += "\n   IP:   " + the_AP_IP_String;
  message += "\n   SSID: " + String(Automat_ssid);
  message += "\n   MAC:  " + WiFi.softAPmacAddress();
  message += "\n\nConnecte au reseau :";
  message += "\n   Local name: " + String(Local_Name) + ".local";
  message += "\n   SSID: " + the_SSID;
  message += "\n   RSSI: " + String(bestRSSI) + " dBm";
  message += "\n   IP:   " + the_IP_String;
  message += "\n   MAC:  " + the_MAC_String;
  message += "\n\nStatus:";
  if (PoolState.ErrorTempSensorInit0) {
    message += "\n   Capteur temperature 0 => ERREUR adress=" + String1wireAddress(Device0_Thermometer);
  }
  if (PoolState.ErrorTempSensorInit1) {
    message += "\n   Capteur temperature 1 => ERREUR adress=" + String1wireAddress(Device1_Thermometer);
  }
  if (PoolState.ErrorTempSensorInit2) {
    message += "\n   Capteur temperature 2 => ERREUR adress=" + String1wireAddress(Device2_Thermometer);
  }
  message +=   "\n   -";
  message += "\n   Temperature air  = " + String(PoolState.AirTemp)      + " deg" + (PoolState.ErrorTempAir ? "(err)" : "")   + " <= device D" + String(AirTempDeviceID());
  message += "\n   Temperature eau  = " + String(PoolState.WaterTemp)    + " deg" + (PoolState.ErrorTempWater ? "(err)" : "") + " <= device D" + String(WaterTempDeviceID());
  message += "\n   Temperature int  = " + String(PoolState.InternalTemp) + " deg" + (PoolState.ErrorTempWater ? "(err)" : "") + " <= device D" + String(InternalTempDeviceID());
  message +=   "\n   -";
  message += "\n   Mode automatique  = " + CurrentModeString();
  if (Automat_Mode.ErrorMode) {
    message += " => ERREUR";
  }
  message += "\n   Position du Volet = " + CurrentCoverPositionString();
  if (Automat_Cmd.ErrorCmd) {
    message += " => ERREUR";
  }
  message +=   "\n   -";
  message += "\n   Seuils : Temp Air = Temp Eau + " + String(Seuil()) + " (ouvre) / " + String(Seuil() - Hysteresis()) + " (ferme)";
  message += "\n\nCommands: ";
  message += "\n   http://" + String(Local_Name) + ".local/update_IDE : mettre a jour le logiciel IDE OTA";
  message += "\n   http://" + String(Local_Name) + ".local/update_WEB : mettre a jour le logiciel WEB OTA";
  message += "\n   http://" + String(Local_Name) + ".local/restart    : redemarrer ESP";
  message += "\n   http://" + String(Local_Name) + ".local/reset_offs : reset temp offsets";
  message += "\n   http://" + String(Local_Name) + ".local/heure_ete  : heure ete;";
  message += "\n   http://" + String(Local_Name) + ".local/heure_hiver: heure hiver";
  message += "\n   http://" + String(Local_Name) + ".local/eau_plus   : offset eau +0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/eau_moins  : offset eau -0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/air_plus   : offset air +0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/air_moins  : offset air -0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/swap_air_eau     : permuter les sondes air et eau";
  message += "\n   http://" + String(Local_Name) + ".local/swap_air_interne : permuter les sondes air et interne";
  message += "\n";
  message += "\n" + StringPreferences();
  //Serial.print(message);
  request->send(200, "text/plain", message);
}

//--------------------------------------------------------------------
void handleRestart_ESP(AsyncWebServerRequest *request) {
  String message = "Restarting the " + String(Local_Name) + "...";
  request->send(200, "text/plain", message);
  printlnD(message);
  restartFlag = true;
}

//--------------------------------------------------------------------
void handleIDE_OTA(AsyncWebServerRequest *request) {
  String message = "Initialisation of software update server OTA via IDE...restart your Arduino IDE to connect...";
  request->send(200, "text/plain", message);
  printlnD(message);
  delay(1000);
  Start_WiFi_IDE_OTA();
}

//--------------------------------------------------------------------
void handleSummerHour(AsyncWebServerRequest *request) {
  String message = "Passage à l'heure d'été...";
  request->send(200, "text/plain", message);
  printlnD(message);
  SetSummerHour(true);
}

//--------------------------------------------------------------------
void handleWinterHour(AsyncWebServerRequest *request) {
  String message = "Passage à l'heure d'hiver...";
  request->send(200, "text/plain", message);
  printlnD(message);
  SetSummerHour(false);
}

//--------------------------------------------------------------------
void handleResetOffsetTemp(AsyncWebServerRequest *request) {
  String message = "Reseting the Offsets Temp on the Arduino EEPROM...";
  request->send(200, "text/plain", message);
  printlnD(message);
  // reseter les offset de mesure températures air et eau
  ResetOffsetPreferences();
}

//--------------------------------------------------------------------
void handleWaterPlus(AsyncWebServerRequest *request) {
  String message = "Offset eau +0.25 degre...";
  request->send(200, "text/plain", message);
  printlnD(message);
  // modifier l'offset de mesure de température
  SetWaterTempOffset(TEMPOFFSETINCREMENT);
}

//--------------------------------------------------------------------
void handleWaterMinus(AsyncWebServerRequest *request) {
  String message = "Offset eau -0.25 degre...";
  request->send(200, "text/plain", message);
  printlnD(message);
  // modifier l'offset de mesure de température
  SetWaterTempOffset(-TEMPOFFSETINCREMENT);
}

//--------------------------------------------------------------------
void handleAirPlus(AsyncWebServerRequest *request) {
  String message = "Offset air +0.25 degre...";
  request->send(200, "text/plain", message);
  printlnD(message);
  // modifier l'offset de mesure de température
  SetAirTempOffset(TEMPOFFSETINCREMENT);
}

//--------------------------------------------------------------------
void handleAirMinus(AsyncWebServerRequest *request) {
  String message = "Offset air -0.25 degre...";
  request->send(200, "text/plain", message);
  printlnD(message);
  // modifier l'offset de mesure de température
  SetAirTempOffset(-TEMPOFFSETINCREMENT);
}

//--------------------------------------------------------------------
void handleSwapAirWater(AsyncWebServerRequest *request) {
  String message = "Permutation des sondes Air et Eau...";
  request->send(200, "text/plain", message);
  printlnD(message);
  int temp = WaterTempDeviceID(); //          Water => temp
  SetWaterTempDeviceID(AirTempDeviceID()); // Air   => Water
  SetAirTempDeviceID(temp); //                temp  => Air
}

//--------------------------------------------------------------------
void handleSwapAirInternal(AsyncWebServerRequest *request) {
  String message = "Permutation des sondes Air et Interne...";
  request->send(200, "text/plain", message);
  printlnD(message);
  int temp = InternalTempDeviceID(); //          Internal => temp
  SetInternalTempDeviceID(AirTempDeviceID()); // Air      => Internal
  SetAirTempDeviceID(temp); //                   temp  => Air
}

//--------------------------------------------------------------------
void handleNotFound(AsyncWebServerRequest *request) {
  //  String message = "File Not Found\n\n";
  //  message += "URL: ";
  //  message += request->url();
  //  message += "\nMethod: ";
  //  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  //  message += "\nArguments: ";
  //  message += request->args();
  //  message += "\n";
  //  for (uint8_t i = 0; i < request->args(); i++) {
  //    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  //  }
  //  request->send(404, "text/plain", message);
  Serial.printf("NOT_FOUND: ");
  if (request->method() == HTTP_GET)
    Serial.printf("GET");
  else if (request->method() == HTTP_POST)
    Serial.printf("POST");
  else if (request->method() == HTTP_DELETE)
    Serial.printf("DELETE");
  else if (request->method() == HTTP_PUT)
    Serial.printf("PUT");
  else if (request->method() == HTTP_PATCH)
    Serial.printf("PATCH");
  else if (request->method() == HTTP_HEAD)
    Serial.printf("HEAD");
  else if (request->method() == HTTP_OPTIONS)
    Serial.printf("OPTIONS");
  else
    Serial.printf("UNKNOWN");
  Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

  if (request->contentLength()) {
    Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
    Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
  }

  int headers = request->headers();
  int i;
  for (i = 0; i < headers; i++) {
    AsyncWebHeader* h = request->getHeader(i);
    Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
  }

  int params = request->params();
  for (i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    if (p->isFile()) {
      Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if (p->isPost()) {
      Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }

  request->send(404);
}

//-----------------------------------------------
//   START THE WEB SERVER
//-----------------------------------------------
void StartWEBserver () {
  // Page WEB de Status
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/HomePage.html", String(), false, processor);
  });

  // Page WEB d'Info textuelles
  server.on("/info", handleTextInfo);

  // Page WEB de Restart ESP
  server.on("/restart", handleRestart_ESP);

  // Page WEB de Update software IDE OTA
  server.on("/update_IDE", handleIDE_OTA);

  // Page WEB de Update software WEB OTA
  //  server.on("/update_WEB", HTTP_GET, []() {
  //    server->sendHeader("Connection", "close");
  //    server->send(200, "text/html", UploadPage);
  //  });
  server.serveStatic("/update_WEB", SPIFFS, "/UploadPage.html");

  // Page WEB de Update software WEB OTA
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST,
  [](AsyncWebServerRequest * request) {
    //    //request->sendHeader("Connection", "close");
    //    request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    //    ESP.restart();
    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    restartFlag = true;  // Tell the main loop to restart the ESP
    request->send(response);
  },
  //  [](AsyncWebServerRequest * request) {
  //    HTTPUpload& upload = request->upload();
  //    if (upload.status == UPLOAD_FILE_START) {
  //      Serial.printf("Update: %s\n", upload.filename.c_str());
  //      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
  //        Update.printError(Serial);
  //      }
  //    } else if (upload.status == UPLOAD_FILE_WRITE) {
  //      /* flashing firmware to ESP*/
  //      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
  //        Update.printError(Serial);
  //      }
  //    } else if (upload.status == UPLOAD_FILE_END) {
  //      if (Update.end(true)) { //true to set the size to the current progress
  //        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
  //      } else {
  //        Update.printError(Serial);
  //      }
  //    }
  //  });

  [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    //Upload handler chunks in data

    if (!index) { // if index == 0 then this is the first frame of data
      Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.setDebugOutput(true);

      // calculate sketch space required for the update
      //  uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      //  if (!Update.begin(maxSketchSpace)) { //start with max available size
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
      //Update.runAsync(true); // tell the updaterClass to run in async mode
    }

    //Write chunked data to the free sketch space
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }

    if (final) { // if the final flag is set then this is the last frame of data
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u B\nRebooting...\n", index + len);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
  });

  // Page WEB de passage à l'heure d'été
  server.on("/heure_ete", handleSummerHour);

  // Page WEB de passage à l'heure d'hiver
  server.on("/heure_hiver", handleWinterHour);

  // Page WEB de reset Offsets températures
  server.on("/reset_offs", handleResetOffsetTemp);

  // Page WEB de Incrément delta T
  server.on("/eau_plus", handleWaterPlus);
  // Page WEB de Incrément delta T
  server.on("/eau_moins", handleWaterMinus);

  // Page WEB de Incrément Hystérésis
  server.on("/air_plus", handleAirPlus);
  // Page WEB de Incrément Hystérésis
  server.on("/air_moins", handleAirMinus);

  // Page WEB de permutation des sondes de température Air et Eau
  server.on("/swap_air_eau", handleSwapAirWater);
  // Page WEB de permutation des sondes de température Air et Interne
  server.on("/swap_air_interne", handleSwapAirInternal);

  // Page WEB d'erreur
  server.onNotFound(handleNotFound);

  // Start TCP (HTTP) server
  server.begin();
  printlnA("HTTP server started");
  DisplayOneMoreLine("HTTP server started", TEXT_ALIGN_LEFT);
}
