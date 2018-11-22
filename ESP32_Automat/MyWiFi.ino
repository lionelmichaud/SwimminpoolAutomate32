//-----------------------------------------------
// . START A WIFI ACCESS POINT
//-----------------------------------------------
boolean StartWiFiSoftAP(Configuration_T Config) {
  printlnA("Wi-Fi access point starting...");

  char ap_password[Config.automat_pwd.length() + 1];
  Config.automat_pwd.toCharArray(ap_password, Config.automat_pwd.length() + 1); // récupère le param dans le tableau de char
  boolean result = WiFi.softAP(Automat_ssid, ap_password); //WiFi.softAP(Automat_ssid, automat_pwd);
  delay(500);

  // clear the display
  display.clear();
  Ypos = 0;

  if (result == true) {
    printlnA("Wi-Fi access point started");
    DisplayOneMoreLine("Wi-Fi access point started", TEXT_ALIGN_LEFT);
  }
  else {
    printlnA("Wi-Fi access point failed!");
    DisplayAlert("Wi-Fi access point failed !");
    display.setFont(ArialMT_Plain_10);
    return false;
  }

  AP_IP = WiFi.softAPIP();
  the_AP_IP_String = String(AP_IP[0]) + "." + String(AP_IP[1]) + "." + String(AP_IP[2]) + "." + String(AP_IP[3]);
  printA("  AP IP address: ");
  printlnA(AP_IP);

  DisplayOneMoreLine("AP IP @ " + the_AP_IP_String, TEXT_ALIGN_LEFT);

  return true;
}

//---------------------------------------------------
// . DEMARRAGE DU SERVEUR WEB DE TELECHARGEMENT OTA
//---------------------------------------------------
boolean Start_WiFi_WEB_OTA() {
  //  httpUpdater.setup(&server);
  //  DisplayOneMoreLine("HTTP Update Server ready", TEXT_ALIGN_LEFT);
  //  debugD(">HTTP Update Server ready! Open http://%s.local/update in your browser\n", Local_Name);
  //  delay(aDelay);
}

//---------------------------------------------------
// . DEMARRAGE DU SERVEUR IDE DE TELECHARGEMENT OTA
//---------------------------------------------------
boolean Start_WiFi_IDE_OTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    printlnA("Start updating " + type);
    events.send("Update Start", "ota");
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 10, "Start updating " + type);
    display.display();
  });

  ArduinoOTA.onEnd([]() {
    printlnA("\nEnd");
    events.send("Update End", "ota");
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
    display.display();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    debugA("Progress: %u%%\r", (progress / (total / 100)));
    char p[32];
    sprintf(p, "Progress: %u%%\n", (progress / (total / 100)));
    events.send(p, "ota");
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    debugE("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) printlnE("Auth Failed")
      else if (error == OTA_BEGIN_ERROR) printlnE("Begin Failed")
        else if (error == OTA_CONNECT_ERROR) printlnE("Connect Failed")
          else if (error == OTA_RECEIVE_ERROR) printlnE("Receive Failed")
            else if (error == OTA_END_ERROR) printlnE("End Failed");
    ESP.restart();
  });

  ArduinoOTA.begin();

  events.onConnect([](AsyncEventSourceClient * client) {
    client->send("hello!", NULL, millis(), 1000);
  });
  server.addHandler(&events);

  printlnA("IDE Update server initialized");
}

//-----------------------------------------------
//   CHECK IF TH WIFI SSID IS ONE OF MINE
//-----------------------------------------------
boolean isMyWiFi(Configuration_T Configuration, String the_ssid) {
  for (int j = 0; j < Configuration.nbWiFiNetworks - 1; ++j)
    if (the_ssid == Configuration.WiFiNetworks[j].ssid) return true;
  return false;
}

//-----------------------------------------------
//   GET THE WIFI PASSWORD IF ITS ONE OF MINE
//-----------------------------------------------
String myWiFiPassword(Configuration_T Configuration, String the_ssid) {
  for (int j = 0; j < Configuration.nbWiFiNetworks - 1; ++j)
    if (the_ssid == Configuration.WiFiNetworks[j].ssid) return Configuration.WiFiNetworks[j].password;
  return "";
}

//-----------------------------------------------
// . CONNECT TO BEST AVAILABLE WIFI AP SIGNAL
//-----------------------------------------------
boolean ConnectToWiFi(Configuration_T Config) {
  const int MaxAttempts = 60;
  //--------------------
  // SCAN WI-FI
  //--------------------
  printlnA("Wi-Fi scan start...");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  printlnA("Wi-Fi scan done");
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  Ypos = 0;
  DisplayOneMoreLine("Wi-Fi scan done", TEXT_ALIGN_LEFT);

  //--------------------------------------------------
  // SELECT BEST WI-FI AND CONNECT TO IT IF IT'S YOURS
  //--------------------------------------------------
  if (n == 0) {
    printlnA(" no Wi-Fi network found !!");
    DisplayAlert("No Wi-Fi network found !!");
    display.setFont(ArialMT_Plain_10);
    return false;
  }
  else
  {
    printA(" "); printA(n);
    printlnA(" Wi-Fi networks found:");
    //DisplayOneMoreLine(String(n) + " Wi-Fi networks found:", TEXT_ALIGN_LEFT);
    bestRSSI = -600;
    selectedWiFi = -1;
    for (int i = 0; i < n; ++i)
    {
      //----------------------
      // FIND BEST RSSI
      //----------------------
      theRSSI = WiFi.RSSI(i);

      // if (WiFi.SSID(i) == MonBWiFi_ssid) or (WiFi.SSID(i) == Garage_ssid)) {
      if (isMyWiFi(Config, WiFi.SSID(i))) {
        //        (WiFi.SSID(i) == MonWiFi_ssid) or
        //        (WiFi.SSID(i) == MonBWiFi_ssid) or
        //        (WiFi.SSID(i) == Pool_ssid) or
        //        (WiFi.SSID(i) == Garage_ssid) ) {
        DisplayOneMoreLine("." + String(i + 1) + ": " + String(WiFi.SSID(i)) + " (RSSI=" + String(theRSSI) + ")", TEXT_ALIGN_LEFT);

        if (theRSSI > bestRSSI) {
          bestRSSI = theRSSI;
          selectedWiFi = i;
        }
      }
      //--------------------------------------------
      // Print SSID and RSSI for each network found
      //--------------------------------------------
      printA("   "); printA(i + 1);
      printA(": ");
      printA(WiFi.SSID(i));
      printA(" (RSSI=");
      printA(theRSSI);
      printlnA(")");
      //printlnA((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
    }
    char selected_ssid[WiFi.SSID(selectedWiFi).length() + 1]; // tableau de char de la taille du String param+1 (caractère de fin de ligne)
    WiFi.SSID(selectedWiFi).toCharArray(selected_ssid, WiFi.SSID(selectedWiFi).length() + 1); // récupère le param dans le tableau de char
    the_SSID = WiFi.SSID(selectedWiFi);
    the_password = myWiFiPassword(Config, the_SSID);

    printlnA("");
    printA("Connecting to Wifi : ");
    printA(selected_ssid);
    printA(" with password : ");
    printlnA(the_password);
    DisplayOneMoreLine("Connecting to : " + String(selected_ssid), TEXT_ALIGN_LEFT);

    //--------------------
    // CONNECTION WI-FI
    //--------------------
    //printlnA(password);
    char selected_password[the_password.length() + 1];
    the_password.toCharArray(selected_password, the_password.length() + 1); // récupère le param dans le tableau de char
    WiFi.begin(selected_ssid, selected_password);
    int connectionAttemps = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      printA(".");
      display.drawProgressBar(4, 54, 120, 6, int((connectionAttemps * 100 ) / MaxAttempts) );
      // display buffer
      display.display();

      connectionAttemps += 1;

      // redémarrer si pas de connection au bout de 30 secondes
      if (connectionAttemps == MaxAttempts) {
        printlnA("Rebooting ESP...");
        DisplayAlert("No connection. Rebooting ESP...");
        ESP.restart();
      }
    }
    //----------------------
    // ALLUMER LA LED ROUGE
    //----------------------
    //digitalWrite(redLEDpin, HIGH);

    printlnA("");
    printlnA("WiFi connected");
    the_IP = WiFi.localIP();
    the_IP_String = String(the_IP[0]) + "." + String(the_IP[1]) + "." + String(the_IP[2]) + "." + String(the_IP[3]);
    printA("  IP address: ");
    printlnA(the_IP);
    WiFi.macAddress(mac);
    the_MAC_String = String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX);
    printA("  MAC address: ");
    printlnA(the_MAC_String);

    // clear the display
    display.clear();
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    // display buffer
    display.display();
    delay(2000);

    // clear the display
    display.clear();
    Ypos = 0;
    DisplayOneMoreLine("WiFi connected", TEXT_ALIGN_LEFT);
    DisplayOneMoreLine("  IP @ " + the_IP_String, TEXT_ALIGN_LEFT);
    DisplayOneMoreLine("  MAC @ " + the_MAC_String, TEXT_ALIGN_LEFT);
    delay(2000);

    //--------------------
    // START MDNS SERVER
    //--------------------
    if (!MDNS.begin(Local_Name)) {
      DisplayAlert("Error setting up MDNS responder !");
      display.setFont(ArialMT_Plain_10);
      printlnA(" Error setting up MDNS responder!");
    } else {
      DisplayOneMoreLine("mDNS responder started", TEXT_ALIGN_LEFT);
      printlnA("mDNS responder started");
    }
    // Add service to MDNS-SD
    delay(1000);
    MDNS.addService("http", "tcp", 80);

    //-----------------------------
    // START THE OTA UPDATE SERVICE
    //-----------------------------
    Start_WiFi_WEB_OTA();
    //Start_WiFi_IDE_OTA(); // exécuter seulement si on veut pouvoir télécharger le logiciel

    //--------------------
    // START MDNS SERVER
    //--------------------
    StartWEBserver();

    return true;
  }
}

void BrowseService(const char * service, const char * proto) {
  debugA("Browsing for service _%s._%s.local. ... ", service, proto);
  int n = MDNS.queryService(service, proto);
  if (n == 0) {
    printlnA("no services found");
  } else {
    printA(n);
    printlnA(" service(s) found");
    for (int i = 0; i < n; ++i) {
      // Print details for each service found
      printA("  ");
      printA(i + 1);
      printA(": ");
      printA(MDNS.hostname(i));
      printA(" (");
      printA(MDNS.IP(i));
      printA(":");
      printA(MDNS.port(i));
      printlnA(")");
    }
  }
  printlnD();
}

void BrowseAllServices() {
  BrowseService("http", "tcp");
  delay(1000);
  BrowseService("arduino", "tcp");
  delay(1000);
  BrowseService("workstation", "tcp");
  delay(1000);
  BrowseService("smb", "tcp");
  delay(1000);
  BrowseService("afpovertcp", "tcp");
  delay(1000);
  BrowseService("ftp", "tcp");
  delay(1000);
  BrowseService("ipp", "tcp");
  delay(1000);
  BrowseService("printer", "tcp");
  delay(10000);
}
