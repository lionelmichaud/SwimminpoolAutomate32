//-----------------------------------------------
// . START A WIFI ACCESS POINT
//-----------------------------------------------
boolean StartWiFiSoftAP() {
#if defined VERBOSE
  Serial.println(">Wi-Fi access point starting...");
  delay(aDelay);
#endif

  boolean result = WiFi.softAP(Automat_ssid, Automat_pwd); //WiFi.softAP(Automat_ssid, Automat_pwd);
  delay(500);

  if (result == true) {
#if defined VERBOSE
    Serial.println(">Wi-Fi access point started");
#endif
    display.drawString(64, Ypos, "Wi-Fi access point started");
    Ypos += fontsize;
    display.display();
  }
  else {
#if defined VERBOSE
    Serial.println(">Wi-Fi access point failed!");
#endif
    DisplayAlert("Wi-Fi access point failed !");
    display.setFont(ArialMT_Plain_10);
    return false;
  }

  AP_IP = WiFi.softAPIP();
  the_AP_IP_String = String(AP_IP[0]) + "." + String(AP_IP[1]) + "." + String(AP_IP[2]) + "." + String(AP_IP[3]);
#if defined VERBOSE
  Serial.print(">  AP IP address: ");
  Serial.println(AP_IP);
#endif

  display.drawString(64, Ypos, "AP IP @ " + the_AP_IP_String);
  Ypos += fontsize;
  display.display();

  return true;
}

//---------------------------------------------------
// . DEMARRAGE DU SERVEUR WEB DE TELECHARGEMENT OTA
//---------------------------------------------------
boolean WiFi_WEB_OTA() {
  //  httpUpdater.setup(&server);
  //  DisplayOneMoreLine("HTTP Update Server ready", TEXT_ALIGN_LEFT);
  //#if defined VERBOSE
  //  Serial.printf(">HTTP Update Server ready! Open http://%s.local/update in your browser\n", Local_Name);
  //  delay(aDelay);
  //#endif
}

//---------------------------------------------------
// . DEMARRAGE DU SERVEUR IDE DE TELECHARGEMENT OTA
//---------------------------------------------------
boolean WiFi_IDE_OTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  //  ArduinoOTA.onStart([]() {
  //    Serial.println("Start");
  //  });
  //  ArduinoOTA.onEnd([]() {
  //    Serial.println("\nEnd");
  //  });
  //  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  //    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  //  });
  //  ArduinoOTA.onError([](ota_error_t error) {
  //    Serial.printf("Error[%u]: ", error);
  //    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
  //    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
  //    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
  //    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
  //    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  //
  //    ESP.restart();
  //  });
  //  ArduinoOTA.begin();
  //#if defined VERBOSE
  //  Serial.println(">IDE Update server initialized");
  //  delay(aDelay);
  //#endif
}

//-----------------------------------------------
// . CONNECT TO BEST AVAILABLE WIFI AP SIGNAL
//-----------------------------------------------
boolean ConnectToWiFi() {
  const int MaxAttempts = 60;
  //--------------------
  // SCAN WI-FI
  //--------------------
#if defined VERBOSE
  Serial.println(">Wi-Fi scan start");
  delay(aDelay);
#endif

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
#if defined VERBOSE
  Serial.println(">Wi-Fi scan done");
  delay(aDelay);
#endif
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  Ypos = 0;
  DisplayOneMoreLine("Wi-Fi scan done", TEXT_ALIGN_LEFT);

  //--------------------------------------------------
  // SELECT BEST WI-FI AND CONNECT TO IT IF IT'S YOURS
  //--------------------------------------------------
  if (n == 0) {
#if defined VERBOSE
    Serial.println("> no Wi-Fi network found !!");
    delay(aDelay);
#endif
    DisplayAlert("No Wi-Fi network found !!");
    display.setFont(ArialMT_Plain_10);
    return false;
  }
  else
  {
#if defined VERBOSE
    Serial.print("> "); Serial.print(n);
    Serial.println(" Wi-Fi networks found:");
    delay(aDelay);
#endif
    //DisplayOneMoreLine(String(n) + " Wi-Fi networks found:", TEXT_ALIGN_LEFT);
    bestRSSI = -600;
    selectedWiFi = -1;
    for (int i = 0; i < n; ++i)
    {
      //----------------------
      // FIND BEST RSSI
      //----------------------
      theRSSI = WiFi.RSSI(i);
      if ( (theRSSI > bestRSSI) and
           //((WiFi.SSID(i) == MonBWiFi_ssid) or (WiFi.SSID(i) == Garage_ssid)) ) {
           (
             (WiFi.SSID(i) == MonWiFi_ssid) or
             (WiFi.SSID(i) == MonBWiFi_ssid) or
             (WiFi.SSID(i) == Pool_ssid) or
             (WiFi.SSID(i) == Garage_ssid)) ) {
        bestRSSI = theRSSI;
        selectedWiFi = i;
        DisplayOneMoreLine("." + String(i + 1) + ": " + String(WiFi.SSID(i)) + " (RSSI=" + String(theRSSI) + ")", TEXT_ALIGN_LEFT);
      }
#if defined VERBOSE
      //--------------------------------------------
      // Print SSID and RSSI for each network found
      //--------------------------------------------
      Serial.print(">   "); Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (RSSI=");
      Serial.print(theRSSI);
      Serial.println(")");
      //Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(aDelay);
#endif
    }
    char selected_ssid[WiFi.SSID(selectedWiFi).length() + 1]; // tableau de char de la taille du String param+1 (caractère de fin de ligne)
    WiFi.SSID(selectedWiFi).toCharArray(selected_ssid, WiFi.SSID(selectedWiFi).length() + 1); // récupère le param dans le tableau de char
    the_SSID = WiFi.SSID(selectedWiFi);

#if defined VERBOSE
    Serial.println("");
    Serial.print(">Connecting to Wifi : ");
    Serial.println(selected_ssid);
    delay(aDelay);
#endif
    DisplayOneMoreLine("Connecting to : " + String(selected_ssid), TEXT_ALIGN_LEFT);

    //--------------------
    // CONNECTION WI-FI
    //--------------------
    //Serial.println(password);
    WiFi.begin(selected_ssid, password);
    int connectionAttemps = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
#if defined VERBOSE
      Serial.print(".");
#endif
      display.drawProgressBar(0, 55, 120, 8, int((connectionAttemps * 100 ) / MaxAttempts) );
      // display buffer
      display.display();

      connectionAttemps += 1;

      // redémarrer si pas de connection au bout de 30 secondes
      if (connectionAttemps == MaxAttempts) {
#if defined VERBOSE
        Serial.println(">Rebooting ESP...");
#endif
        DisplayAlert("No connection. Rebooting ESP...");
        ESP.restart();
      }
    }
    //----------------------
    // ALLUMER LA LED ROUGE
    //----------------------
    //digitalWrite(redLEDpin, HIGH);

#if defined VERBOSE
    Serial.println("");
    Serial.println(">WiFi connected");
    delay(aDelay);
    the_IP = WiFi.localIP();
    the_IP_String = String(the_IP[0]) + "." + String(the_IP[1]) + "." + String(the_IP[2]) + "." + String(the_IP[3]);
    Serial.print(">  IP address: ");
    Serial.println(the_IP);
    delay(aDelay);
    WiFi.macAddress(mac);
    the_MAC_String = String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX);
    Serial.print(">  MAC address: ");
    Serial.println(the_MAC_String);
    delay(aDelay);
#endif

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
    delay(1000);
    if (!MDNS.begin(Local_Name)) {
      DisplayAlert("Error setting up MDNS responder !");
      display.setFont(ArialMT_Plain_10);
#if defined VERBOSE
      Serial.println("> Error setting up MDNS responder!");
#endif
    } else {
      DisplayOneMoreLine("mDNS responder started", TEXT_ALIGN_LEFT);
#if defined VERBOSE
      Serial.println(">mDNS responder started");
#endif
    }
    // Add service to MDNS-SD
    delay(1000);
    MDNS.addService("http", "tcp", 80);

    //-----------------------------
    // START THE OTA UPDATE SERVICE
    //-----------------------------
    WiFi_WEB_OTA();
    WiFi_IDE_OTA(); // exécuter seulement si on veut pouvoir télécharger le logiciel

    //--------------------
    // START MDNS SERVER
    //--------------------
    //StartWEBserver();

    return true;
  }
}

void BrowseService(const char * service, const char * proto) {
  Serial.printf("Browsing for service _%s._%s.local. ... ", service, proto);
  int n = MDNS.queryService(service, proto);
  if (n == 0) {
    Serial.println("no services found");
  } else {
    Serial.print(n);
    Serial.println(" service(s) found");
    for (int i = 0; i < n; ++i) {
      // Print details for each service found
      Serial.print("  ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");
    }
  }
  Serial.println();
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
