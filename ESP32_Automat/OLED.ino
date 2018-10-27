//------------------------------------------------
// INITIALISATION OLED DISPLAY AND USER INTERFACE
//------------------------------------------------
void initializeOLED() {
  Serial.println("OLED Init...");

  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(30);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  // Initialising the UI will init the display too.
  display.init();
  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  // clear the display
  display.clear();
}

//------------------------------------------------
// AFFICHAGE DU BANDEAU FIXE
//------------------------------------------------
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  // left
  if (WiFi.status() == WL_CONNECTED) display->drawXbm(0, 0, wifi_02_width, wifi_02_height, wifi_02_bits);

  // center left
  if (!PoolState.ErrorTemp0) {
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->setFont(ArialMT_Plain_10);
    display->drawString(43, 0, "D0");
    display->drawXbm(43, 0, net_down_03_width, net_down_03_height, net_down_03_bits);
  }

  // center right
  if (!PoolState.ErrorTemp1) {
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->setFont(ArialMT_Plain_10);
    display->drawString(72, 0, "D1");
    display->drawXbm(72, 0, net_down_03_width, net_down_03_height, net_down_03_bits);
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

  display->drawString(0 + x, 10 + y, "SW: " + String(SOFTWARE) + " - " + String(VERSION));
  display->drawString(0 + x, 20 + y, "Compiled :    " + String(__DATE__));
  display->drawString(0 + x, 30 + y, "Dallas devices : " + String(DallasDeviceCount));
  if (!PoolState.ErrorTemp0) display->drawString( 0 + x, 40 + y, "  D0: " + String1wireAddress(Device0_Thermometer));
  if (!PoolState.ErrorTemp1) display->drawString(64 + x, 40 + y, "D1: " + String1wireAddress(Device1_Thermometer));
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

void drawDeviceInfoTemperatures(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);

  display->drawString(64 + x, 14 + y,      "AIR : " + String(PoolState.AirTemp) + "°C");
  display->drawString(64 + x, 14 + 20 + y, "EAU : " + String(PoolState.WaterTemp) + "°C");
}

void drawDeviceInfoStatus(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  //  display->drawString(0 + x, 10 + y, "Volume Gaz : " + String(Gaz_M3) + " m3");

}

//------------------------------------------------
// AFFICHAGE UNE LIGNE DE PLUS VERS LE BAS
//------------------------------------------------
void DisplayOneMoreLine(String line, OLEDDISPLAY_TEXT_ALIGNMENT textAlignment) {
  display.setTextAlignment(textAlignment);
  display.drawString((textAlignment == TEXT_ALIGN_CENTER) ? 64 : 0, Ypos, line);
  Ypos += fontsize;
  display.display();
}

//------------------------------------------------
// AFFICHAGE MESSAGE D'ALERTE
//------------------------------------------------
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

  // clear the display
  display.clear();
  display.setFont(ArialMT_Plain_10);
}

//**********************************************
//  function to display a 1-Wire device address
//**********************************************
void Display1wireAddress(String label, DeviceAddress deviceAddress)
{
  String address = label;
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) address += "0";
    address += String(deviceAddress[i], HEX);
  }
  DisplayOneMoreLine(address, TEXT_ALIGN_LEFT);
}
