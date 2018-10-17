void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  // left
  if (WiFi.status() == WL_CONNECTED) display->drawXbm(0, 0, wifi_02_width, wifi_02_height, wifi_02_bits);

  // center left
  //if (true || true) {
//  if (INDEX1wh != 0 || INDEX2wh != 0 || INDEX3wh != 0 || INDEX4wh != 0 || INDEX5wh != 0 || INDEX6wh != 0 || papp != 0) {
//    display->setTextAlignment(TEXT_ALIGN_RIGHT);
//    display->setFont(ArialMT_Plain_10);
//    display->drawString(43, 0, "Linky");
//    display->drawXbm(43, 0, net_up_03_width, net_up_03_height, net_up_03_bits);
//  }

  // center right
  if (SoftwareString != "") {
    //if (true) {
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->setFont(ArialMT_Plain_10);
    display->drawString(74, 0, "Uno");
    display->drawXbm(74, 0, net_down_03_width, net_down_03_height, net_down_03_bits);
  }

  // right
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, TimeNTP);
}

void drawPageSoftwareInfo(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  display->drawString(0 + x, 10 + y, "SW: " + SoftwareString);
  display->drawString(0 + x, 20 + y, "SW: " + String(SOFTWARE) + "-" + String(VERSION));
//  uint32_t chipId = ESP.getFlashChipId();
  display->drawString(0 + x, 30 + y, "Compiled :    " + String(__DATE__));
//  display->drawString(0 + x, 40 + y, "Free space:  " + String(ESP.getFreeSketchSpace() / 1024) + " kByte");
}

void drawPageWiFi_AP_Info(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  display->drawString(0 + x, 10 + y, "Wi-Fi Access Point: ");
  display->drawString(0 + x, 20 + y, "  SSID:");
  display->drawString(38 + x, 20 + y, String(Automat_ssid));
  display->drawString(0 + x, 30 + y, "  IP :");
  display->drawString(38 + x, 30 + y, the_AP_IP_String );
  display->drawString(0 + x, 40 + y, "  MAC :");
  display->drawString(38 + x, 40 + y, WiFi.softAPmacAddress());
}

void drawPageWiFi_ST_Info(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  display->drawString(0 + x, 10 + y, "Wi-Fi Station: " + the_SSID);
  display->drawString(0 + x, 20 + y, "  RSSI:");
  display->drawString(38 + x, 20 + y, String(bestRSSI) + " dBm");
  display->drawString(0 + x, 30 + y, "  IP :");
  display->drawString(38 + x, 30 + y, the_IP_String);
  display->drawString(0 + x, 40 + y, "  MAC :");
  display->drawString(38 + x, 40 + y, the_MAC_String);
}

void drawDeviceInfoElec(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

//  display->drawString( 0 + x, 10 + y, "Bleu:");
//  display->drawString(30 + x, 10 + y, "C:" + String(INDEX1wh));
//  display->drawString(80 + x, 10 + y, "P:" + String(INDEX2wh));
//  display->drawString( 0 + x, 20 + y, "Blan:");
//  display->drawString(30 + x, 20 + y, "C: " + String(INDEX3wh));
//  display->drawString(80 + x, 20 + y, "P: " + String(INDEX4wh));
//  display->drawString( 0 + x, 30 + y, "Roug:");
//  display->drawString(30 + x, 30 + y, "C: " + String(INDEX5wh));
//  display->drawString(80 + x, 30 + y, "P: " + String(INDEX6wh));
//  display->drawString( 0 + x, 40 + y, "In : " + String(amps) + " A");
//  display->drawString(64 + x, 40 + y, "Pw : " + String(papp) + " VA");
}

void drawDeviceInfoGaz(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

//  display->drawString(0 + x, 10 + y, "Volume Gaz : " + String(Gaz_M3) + " m3");

}

void DisplayOneMoreLine(String line, OLEDDISPLAY_TEXT_ALIGNMENT textAlignment) {
  display.setTextAlignment(textAlignment);
  display.drawString((textAlignment == TEXT_ALIGN_CENTER) ? 64 : 0, Ypos, line);
  Ypos += fontsize;
  display.display();
}

void DisplayAlert(String AlertText) {
  // clear the display
  display.clear();

  // write ALERT to display buffer
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "ALERT");
  display.display();
  delay(2000);

  // write alert message to display buffer
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawStringMaxWidth(64, 0, 128, AlertText);

  // display buffer
  display.display();
  delay(5000);
}
