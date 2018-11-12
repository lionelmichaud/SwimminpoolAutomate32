void ResetOffsetPreferences();

//-------------------------------------------------------
// WEB PAGE FOR SLECTING THE BIN FILE TO UPLOAD TO ESP32
//-------------------------------------------------------
//const char* serverIndex = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
//                          "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
//                          "<input type='file' name='update'>"
//                          "<input type='submit' value='Update'>"
//                          "</form>"
//                          "<div id='prg'>progress: 0%</div>"
//                          "<script>"
//                          "$('form').submit(function(e){"
//                          "e.preventDefault();"
//                          "var form = $('#upload_form')[0];"
//                          "var data = new FormData(form);"
//                          " $.ajax({"
//                          "url: '/update',"
//                          "type: 'POST',"
//                          "data: data,"
//                          "contentType: false,"
//                          "processData:false,"
//                          "xhr: function() {"
//                          "var xhr = new window.XMLHttpRequest();"
//                          "xhr.upload.addEventListener('progress', function(evt) {"
//                          "if (evt.lengthComputable) {"
//                          "var per = evt.loaded / evt.total;"
//                          "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
//                          "}"
//                          "}, false);"
//                          "return xhr;"
//                          "},"
//                          "success:function(d, s) {"
//                          "console.log('success!')"
//                          "},"
//                          "error: function (a, b, c) {"
//                          "}"
//                          "});"
//                          "});"
//                          "</script>";

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

String getPageSoftwareInfo() {
  FlashMode_t ideMode = ESP.getFlashChipMode();
  //uint32_t chipId = ESP.getFlashChipId();

  String page = "";
  page += "<div class='row'>";

  page +=   "<div class='col-md-6'>";
  page +=      "<p class='lead'>";
  page +=         "<strong>ESP Info</strong>";
  page +=      "</p>";
  page +=      "<div>";
  page +=        "<span>Logiciel : " + String(SOFTWARE) + " - " + String(VERSION) + "</span>";
  page +=      "</div>";
  page +=      "<ul>";
  //  page +=         "<li>";
  //  page +=           "ESP Chip ID:     " + String(chipId);
  //  page +=         "</li>";
  //  page +=         "<li>";
  //  page +=           "Flash size: " + String(ESP.getFlashChipRealSize() / 1024) + " kByte";
  //  page +=           "ESP Chip ID Flash size: " + getFlashChipSizeByChipId(chipId);
  //  page +=         "</li>";
  //  page +=         "<li>";
  //  page +=            "Free sketch space: " + String(ESP.getFreeSketchSpace() / 1024) + " kByte";
  //  page +=         "</li>";
  page +=         "<li>";
  page +=           "Flash clock: " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz";
  page +=         "</li>";
  page +=       "</ul>";
  page +=    "</div>";

  page += "</div>";

  return page;
}

String getPageESPInfo() {
  String page = "";
  page += "<div class='row'>";

  page +=   "<div class='col-md-6'>";
  page +=     "<p class='lead'>";
  page +=         "<strong>Wi-Fi Access Point Info</strong>";
  page +=      "</p>";
  page +=      "<div>";
  page +=         "<span>IP : " + the_AP_IP_String + "</span>";
  page +=      "</div>";
  page +=      "<ul>";
  page +=         "<li>";
  page +=            "SSID: " + String(Automat_ssid);
  page +=         "</li>";
  page +=         "<li>";
  page +=            "MAC : " + WiFi.softAPmacAddress();
  page +=         "</li>";
  page +=      "</ul>";
  page +=   "</div>";

  page +=   "<div class='col-md-6'>";
  page +=     "<p class='lead'>";
  page +=         "<strong>Wi-Fi Station Info</strong>";
  page +=      "</p>";
  page +=      "<div>";
  page +=          "<span>IP : " + the_IP_String + "</span>";
  page +=       "</div>";
  page +=       "<ul>";
  page +=          "<li>";
  page +=             "Local name: " + String(Local_Name) + ".local";
  page +=          "</li>";
  page +=          "<li>";
  page +=             "SSID: " + the_SSID;
  page +=          "</li>";
  page +=          "<li>";
  page +=             "RSSI: " + String(bestRSSI) + " dBm";
  page +=          "</li>";
  page +=          "<li>";
  page +=             "MAC: " + the_MAC_String;
  page +=          "</li>";
  page +=       "</ul>";
  page +=    "</div>";

  page += "</div>";

  return page;
}

String getPageDeviceInfo() {
  String page = "";
  page += "<div class='row'>";

  page +=   "<div class='col-md-12'>";
  page +=     "<h3 class='text-muted'>";
  page +=       "<em>Etat de la piscine</em>";
  page +=     "</h3>";
  page +=     "<a class='nav-link btn btn-info' href='#'>Temps NTP <span class='badge badge-light'>" + DateNTP + " / " + TimeNTP + "</span></a><br>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Temp air <span class='badge badge-light'>" + String(PoolState.AirTemp) + " deg" + (PoolState.ErrorTempAir ? "*" : "") + "</span></a>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Temp eau <span class='badge badge-light'>" + String(PoolState.WaterTemp) + " deg" + (PoolState.ErrorTempWater ? "*" : "") + "</span></a><br>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Mode automatique <span class='badge badge-light'>" + CurrentModeString() + "</span></a>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Position du Volet <span class='badge badge-light'>" + CurrentCoverPositionString() + "</span></a>";
  page +=     "<ul>";
  page +=       "Seuils : Temp Air = Temp Eau + " + String(Seuil()) + " (ouvre) / " + String(Seuil() - Hysteresis()) + " (ferme)";
  page +=     "</ul>";
  page +=   "</div>";

  page += "</div>";

  return page;
}

String getPageCommands() {
  String page = "";
  page += "<div class='row'>";

  page +=   "<div class='col-md-12'>";
  page +=       "<h3 class='text-muted'>";
  page +=         "<em>Commandes disponibles</em>";
  page +=       "</h3>";
  page +=       "<a href=/update_IDE target=blank class='btn btn-danger'  >Mettre a jour le logiciel IDE OTA</a><br>";
  page +=       "<a href=/update_WEB target=blank class='btn btn-danger'  >Mettre a jour le logiciel WEB OTA</a><br>";
  page +=       "<a href=/restart    target=blank class='btn btn-danger'  >Red&eacute;marrer ESP</a><br>";
  page +=       "<a href=/heure_ete  target=blank class='btn btn-warning' >Heure &eacute;t&eacute;</a><br>";
  page +=       "<a href=/heure_hiver target=blank class='btn btn-warning' >Heure hiver</a><br>";
  page +=       "<a href=/reset_offs target=blank class='btn btn-warning' >Reset temp offsets sur Arduino</a><br>";
  page +=       "<a href=/info       target=blank class='btn btn-success' >Obtenir ces informations au format text</a><br>";

  page +=       "<h5 class='text-left text-primary'>";
  page +=         "Offset Temperature Air : " + String(AirTempOffset());
  page +=       "</h5>";
  //  page +=       "<div class='row'>";
  //  page +=       "  <div class='col-md-6'>";
  page +=       "<a class='nav-link btn btn-primary' href=/air_plus target=blank>+ <span class='badge badge-light'>0.25 deg</span></a>";
  //  page +=       "  </div>";
  //  page +=       "  <div class='col-md-6'>";
  page +=       "<a class='nav-link btn btn-primary' href=/air_moins target=blank>- <span class='badge badge-light'>0.25 deg</span></a><br>";
  //  page +=       "  </div>";
  //  page +=       "</div>";
  page +=       "<h5 class='text-left text-primary'>";
  page +=         "Offset Temperature Eau : " + String(WaterTempOffset());
  page +=       "</h5>";
  //  page +=       "<div class='row'>";
  //  page +=       "  <div class='col-md-6'>";
  page +=       "<a class='nav-link btn btn-primary' href=/eau_plus target=blank>+ <span class='badge badge-light'>0.25 deg</span></a>";
  //  page +=       "  </div>";
  //  page +=       "  <div class='col-md-6'>";
  page +=       "<a class='nav-link btn btn-primary' href=/eau_moins target=blank>- <span class='badge badge-light'>0.25 deg</span></a><br>";
  //  page +=       "  </div>";
  //  page +=       "</div>";
  page +=   "</div>";

  page += "</div>";

  return page;
}

String getPage() {
  const String header = "<!DOCTYPE html>"
                        "<html lang='fr'>"
                        "<head>"
                        "<meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1'/>"
                        "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>"
                        "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>"
                        "<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>"
                        "</head>"
                        "<body>"
                        "<div class='container-fluid'>"
                        "<div class='row'>"
                        "<div class='col-md-12'>"
                        "<div class='page-header'>"
                        "<h1>Automate de la Piscine</h1>"
                        "</div>"
                        "</div>"
                        "</div>";

  String page = header;

  page += getPageSoftwareInfo();
  page += getPageESPInfo();
  page += getPageDeviceInfo();
  page += getPageCommands();

  page += "</div>";
  page += "</body>";

  page += "</html>";
  return page;
}

//--------------------------------------------------------------------
void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", getPage());
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
  message += "\n   Temperature air  = " + String(PoolState.AirTemp) + " deg" + (PoolState.ErrorTempAir ? "*" : "");
  message += "\n   Temperature eau  = " + String(PoolState.WaterTemp) + " deg" + (PoolState.ErrorTempWater ? "*" : "");
  message += "\n   Mode automatique  = " + CurrentModeString();
  if (Automat_Mode.ErrorMode) {
    message += " => ERREUR";
  }
  message += "\n   Position du Volet = " + CurrentCoverPositionString();
  if (Automat_Cmd.ErrorCmd) {
    message += " => ERREUR";
  }
  message += "\n   Seuils : Temp Air = Temp Eau + " + String(Seuil()) + " (ouvre) / " + String(Seuil() - Hysteresis()) + " (ferme)";
  message += "\n\nCommands: ";
  message += "\n   http://" + String(Local_Name) + ".local/update_IDE : mettre a jour le logiciel IDE OTA";
  message += "\n   http://" + String(Local_Name) + ".local/update_WEB : mettre a jour le logiciel WEB OTA";
  message += "\n   http://" + String(Local_Name) + ".local/restart    : red&eacute;marrer ESP";
  message += "\n   http://" + String(Local_Name) + ".local/reset_offs : reset temp offsets";
  message += "\n   http://" + String(Local_Name) + ".local/heure_ete  : heure &eacute;t&eacute;";
  message += "\n   http://" + String(Local_Name) + ".local/heure_hiver: heure hiver";
  message += "\n   http://" + String(Local_Name) + ".local/eau_plus   : offeset eau +0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/eau_moins  : offeset eau -0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/air_plus   : offeset air +0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/air_moins  : offeset air -0.25 degre";
  message += "\n";
  //Serial.print(message);
  request->send(200, "text/plain", message);
}

//--------------------------------------------------------------------
void handleRestart_ESP(AsyncWebServerRequest *request) {
  String message = "Restarting the " + String(Local_Name) + "...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  restartFlag = true;
}

//--------------------------------------------------------------------
void handleIDE_OTA(AsyncWebServerRequest *request) {
  String message = "Initialisation of software update server OTA via IDE...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  delay(1000);
  Start_WiFi_IDE_OTA();
}

//--------------------------------------------------------------------
void handleSummerHour(AsyncWebServerRequest *request) {
  String message = "Passage à l'heure d'été...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  SetSummerHour(true);
}

//--------------------------------------------------------------------
void handleWinterHour(AsyncWebServerRequest *request) {
  String message = "Passage à l'heure d'hiver...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  SetSummerHour(false);
}

//--------------------------------------------------------------------
void handleResetOffsetTemp(AsyncWebServerRequest *request) {
  String message = "Reseting the Offsets Temp on the Arduino EEPROM...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  // reseter les offset de mesure températures air et eau
  ResetOffsetPreferences();
}

//--------------------------------------------------------------------
void handleWaterPlus(AsyncWebServerRequest *request) {
  String message = "Offset eau +0.25 degre...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  // modifier l'offset de mesure de température
  SetWaterTempOffset(TEMPOFFSETINCREMENT);
}

//--------------------------------------------------------------------
void handleWaterMinus(AsyncWebServerRequest *request) {
  String message = "Offset eau -0.25 degre...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  // modifier l'offset de mesure de température
  SetWaterTempOffset(-TEMPOFFSETINCREMENT);
}

//--------------------------------------------------------------------
void handleAirPlus(AsyncWebServerRequest *request) {
  String message = "Offset air +0.25 degre...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  // modifier l'offset de mesure de température
  SetAirTempOffset(TEMPOFFSETINCREMENT);
}

//--------------------------------------------------------------------
void handleAirMinus(AsyncWebServerRequest *request) {
  String message = "Offset air -0.25 degre...";
  request->send(200, "text/plain", message);
  Serial.println(message);
  // modifier l'offset de mesure de température
  SetAirTempOffset(-TEMPOFFSETINCREMENT);
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
  server.on("/", handleRoot);

  // Page WEB d'Info textuelles
  server.on("/info", handleTextInfo);

  // Page WEB de Restart ESP
  server.on("/restart", handleRestart_ESP);

  // Page WEB de Update software IDE OTA
  server.on("/update_IDE", handleIDE_OTA);

  // Page WEB de Update software WEB OTA
  //  server.on("/update_WEB", HTTP_GET, []() {
  //    server->sendHeader("Connection", "close");
  //    server->send(200, "text/html", serverIndex);
  //  });
  server.serveStatic("/update_WEB", SPIFFS, "/serverIndex.html");

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

  // Page WEB d'erreur
  server.onNotFound(handleNotFound);

  // Start TCP (HTTP) server
  server.begin();
#if defined VERBOSE
  Serial.println("HTTP server started");
#endif
  DisplayOneMoreLine("HTTP server started", TEXT_ALIGN_LEFT);
}
