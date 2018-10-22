//-------------------------------------------------------
// WEB PAGE FOR SLECTING THE BIN FILE TO UPLOAD TO ESP32
//-------------------------------------------------------
const char* serverIndex = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
                          "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
                          "<input type='file' name='update'>"
                          "<input type='submit' value='Update'>"
                          "</form>"
                          "<div id='prg'>progress: 0%</div>"
                          "<script>"
                          "$('form').submit(function(e){"
                          "e.preventDefault();"
                          "var form = $('#upload_form')[0];"
                          "var data = new FormData(form);"
                          " $.ajax({"
                          "url: '/update',"
                          "type: 'POST',"
                          "data: data,"
                          "contentType: false,"
                          "processData:false,"
                          "xhr: function() {"
                          "var xhr = new window.XMLHttpRequest();"
                          "xhr.upload.addEventListener('progress', function(evt) {"
                          "if (evt.lengthComputable) {"
                          "var per = evt.loaded / evt.total;"
                          "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
                          "}"
                          "}, false);"
                          "return xhr;"
                          "},"
                          "success:function(d, s) {"
                          "console.log('success!')"
                          "},"
                          "error: function (a, b, c) {"
                          "}"
                          "});"
                          "});"
                          "</script>";

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
  page +=     "<a class='nav-link btn btn-info' href='#'>Heure <span class='badge badge-light'>" + TimeNTP + "</span></a><br>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Temp air <span class='badge badge-light'>" + String(PoolState.AirTemp) + " deg" + "</span></a>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Temp eau <span class='badge badge-light'>" + String(PoolState.WaterTemp) + " deg" + "</span></a><br>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Temp int <span class='badge badge-light'>" + String(PoolState.IntTemp) + " deg" + "</span></a>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Temp max int <span class='badge badge-light'>" + String(PoolState.MaxIntTemp) + " deg" + "</span></a><br>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Mode automatique <span class='badge badge-light'>" + PoolState.AutomateString + "</span></a>";
  page +=     "<a class='nav-link btn btn-primary' href='#'>Position du Volet <span class='badge badge-light'>" + PoolState.VoletString + "</span></a>";
  page +=     "<ul>";
  page +=       "Seuils : Temp Air = Temp Eau + " + String(AutomatParam.Seuil) + " (ouvre) / " + String(AutomatParam.Seuil - AutomatParam.Hysteresis) + " (ferme)";
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
  page +=       "<a href=/binfile    target=blank class='btn btn-danger'  >Mettre a jour le logiciel WEB OTA</a><br>";
  page +=       "<a href=/restart_e  target=blank class='btn btn-danger'  >Red&eacute;marrer ESP</a><br>";
  page +=       "<a href=/restart_a  target=blank class='btn btn-danger'  >Red&eacute;marrer Arduino</a><br>";
  page +=       "<a href=/reset_tint target=blank class='btn btn-warning' >Reset temp max int sur Arduino</a><br>";
  page +=       "<a href=/reset_offs target=blank class='btn btn-warning' >Reset temp offsets sur Arduino</a><br>";
  page +=       "<a href=/info       target=blank class='btn btn-success' >Obtenir ces informations au format text</a><br>";

  page +=       "<h5 class='text-left text-primary'>";
  page +=         "Offset Temperature Air : " + String(AutomatParam.OffsetAir);
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
  page +=         "Offset Temperature Eau : " + String(AutomatParam.OffsetEau);
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
void handleRoot() {
  server.send(200, "text/html", getPage());
}

//--------------------------------------------------------------------
void handleTextInfo() {
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
  message += "\n\nTime NTP:" + TimeNTP;
  message += "\n\nAccess point:";
  message += "\n   IP:   " + the_AP_IP_String;
  message += "\n\nConnecte au reseau :";
  message += "\n   Local name: " + String(Local_Name) + ".local";
  message += "\n   SSID: " + the_SSID;
  message += "\n   RSSI: " + String(bestRSSI) + " dBm";
  message += "\n   IP:   " + the_IP_String;
  message += "\n   MAC:  " + the_MAC_String;
  message += "\n\nStatus:";
  message += "\n   Temperature air  = " + String(PoolState.AirTemp) + " deg";
  message += "\n   Temperature eau  = " + String(PoolState.WaterTemp) + " deg";
  message += "\n   Temperature interne  = " + String(PoolState.IntTemp) + " deg";
  message += "\n   Temperature interne maximale atteinte = " + String(PoolState.MaxIntTemp) + " deg";
  message += "\n   Mode automatique  = " + PoolState.AutomateString;
  message += "\n   Position du Volet = " + PoolState.VoletString;
  message += "\n   Seuils : Temp Air = Temp Eau + " + String(AutomatParam.Seuil) + " (ouvre) / " + String(AutomatParam.Seuil - AutomatParam.Hysteresis) + " (ferme)";
  message += "\n\nCommands: ";
  message += "\n   http://" + String(Local_Name) + ".local/update_IDE : mettre a jour le logiciel IDE OTA";
  message += "\n   http://" + String(Local_Name) + ".local/binfile    : mettre a jour le logiciel WEB OTA";
  message += "\n   http://" + String(Local_Name) + ".local/restart_e  : redemarrer ESP";
  message += "\n   http://" + String(Local_Name) + ".local/restart_a  : redemarrer Arduino";
  message += "\n   http://" + String(Local_Name) + ".local/reset_tint : reset temp max int sur Arduino";
  message += "\n   http://" + String(Local_Name) + ".local/reset_offs : reset temp offsets sur Arduino";
  message += "\n   http://" + String(Local_Name) + ".local/eau_plus   : offeset eau +0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/eau_moins  : offeset eau -0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/air_plus   : offeset air +0.25 degre";
  message += "\n   http://" + String(Local_Name) + ".local/air_moins  : offeset air -0.25 degre";
  message += "\n";
  //Serial.print(message);
  server.send(200, "text/plain", message);
}

//--------------------------------------------------------------------
void handleIDE_OTA() {
  String message = "Initialisation of software update server OTA via IDE...";
  server.send(200, "text/plain", message);
  Serial.println(">Initialisation of software update server OTA via IDE...");
  delay(1000);
  Start_WiFi_IDE_OTA();
}

//--------------------------------------------------------------------
void handleRestart_ESP() {
  restartFlag = true;
  String message = "Restarting the " + String(Local_Name) + "...";
  server.send(200, "text/plain", message);
}

//--------------------------------------------------------------------
void handleRestart_Arduino() {
  String message = "Restarting Arduino...";
  server.send(200, "text/plain", message);
  Serial.println(">Reset Arduino");
}

//--------------------------------------------------------------------
void handleResetMaxIntTemp() {
  String message = "Reseting the Max Internal Temp on the Arduino EEPROM...";
  server.send(200, "text/plain", message);
  Serial.println(">Reset MaxIntTemp");
}

//--------------------------------------------------------------------
void handleResetOffsetTemp() {
  String message = "Reseting the Offsets Temp on the Arduino EEPROM...";
  server.send(200, "text/plain", message);
  Serial.println(">Reset OffsetTemp");
}

//--------------------------------------------------------------------
void handleWaterPlus() {
  String message = "Offset eau +0.25 degre...";
  server.send(200, "text/plain", message);
  Serial.println(">Offset Water Plus");
}

//--------------------------------------------------------------------
void handleWaterMinus() {
  String message = "Offset eau -0.25 degre...";
  server.send(200, "text/plain", message);
  Serial.println(">Offset Water Moins");
}

//--------------------------------------------------------------------
void handleAirPlus() {
  String message = "Offset air +0.25 degre...";
  server.send(200, "text/plain", message);
  Serial.println(">Offset Air Plus");
}

//--------------------------------------------------------------------
void handleAirMinus() {
  String message = "Offset air -0.25 degre...";
  server.send(200, "text/plain", message);
  Serial.println(">Offset Air Moins");
}

//--------------------------------------------------------------------
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
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
  server.on("/restart_e", handleRestart_ESP);

  // Page WEB de Restart Arduino
  //server.on("/restart_a", handleRestart_Arduino);

  // Page WEB de Update software IDE OTA
  server.on("/update_IDE", handleIDE_OTA);

  // Page WEB de Update software WEB OTA
  server.on("/binfile", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

  // Page WEB de Update software WEB OTA
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK"); ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  // Page WEB de reset MaxIntTemp
  server.on("/reset_tint", handleResetMaxIntTemp);

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

  // Page WEB de test
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  // Page WEB d'erreur
  server.onNotFound(handleNotFound);

  // Start TCP (HTTP) server
  server.begin();
#if defined VERBOSE
  Serial.println(">HTTP server started");
  delay(aDelay);
#endif
  DisplayOneMoreLine("HTTP server started", TEXT_ALIGN_LEFT);
}
