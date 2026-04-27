//-------------------------------
// initialize IO ports
//-------------------------------
void InitializeIO() {
  printlnA("Initializing I/O...");
  // inputs
  //    pinMode(pIntTempR, INPUT);
  pinMode(pAutoSwitch, INPUT_PULLUP);
  // outputs
  pinMode(pAutoLED, OUTPUT);
  pinMode(pTempLEDred, OUTPUT);
  pinMode(pTempLEDgreen, OUTPUT);
  pinMode(pTempLEDblue, OUTPUT);
  pinMode(pRelay1, OUTPUT);
  pinMode(pRelay2, OUTPUT);
  //    open relays
  digitalWrite(pRelay1, Relay1);
  digitalWrite(pRelay2, Relay2);

  //    switch off Auto LED
  digitalWrite(pAutoLED, AutoLED);

  //    configure LED PWM (ESP32 core v3: ledcAttach remplace ledcSetup+ledcAttachPin)
  ledcAttach(pTempLEDred,   LEDfreq, LEDres);
  ledcAttach(pTempLEDgreen, LEDfreq, LEDres);
  ledcAttach(pTempLEDblue,  LEDfreq, LEDres);
  //    switch off Water Temperature LED color between red (cold) and green (hot):
  ledcWrite(pTempLEDred,   0);
  ledcWrite(pTempLEDgreen, 0);
  ledcWrite(pTempLEDblue,  0);
}


//-------------------------------------------------
//  blink Red Auto LED when in AUTOMATIC Mode
//-------------------------------------------------
void BlinkRedAutoLED ()
{
  if (PoolState.ErrorTempSensorInit || PoolState.ErrorConfig)
    // Allumer la LED rouge en fixe
    digitalWrite(pAutoLED, HIGH);

  else if (Automat_Mode.ModeState == AUTOMATIC) {
    // Faire clignoter la LED rouge
    AutoLED = !AutoLED;
    digitalWrite(pAutoLED, AutoLED);
  }
  else
    // Eteindre la LED rouge
    digitalWrite(pAutoLED, LOW);
}

//-------------------------------------------------
// MANAGE RELAY : AUTOMATIC || MANUAL
//  RELAY_OPEN (HIGH) = repos, RELAY_CLOSED (LOW) = actif
//-------------------------------------------------
boolean SwitchRelayAutoManu (int State) {
  switch (State) {

    case MANUAL :
      // ouvrir le Relay1 pour etre sur de se retrouver en command manuelle
      Relay1 = RELAY_OPEN;
      digitalWrite(pRelay1, Relay1);
      return true;

    case AUTOMATIC :
      // fermer le Relay1 pour passer en automatique
      Relay1 = RELAY_CLOSED;
      digitalWrite(pRelay1, Relay1);
      return true;

    default:
      // erreur
      return false;
  }
}

//-------------------------------------------------
// MANAGE RELAY : OPEN || CLOSE
//  RELAY_OPEN (HIGH) = repos, RELAY_CLOSED (LOW) = actif
//-------------------------------------------------
boolean SwitchRelayOpenCloseCover (int State) {
  switch (State) {

    case CLOSE_CMD_ACTIVATED :
      // ouvrir le Relay2 => Fermeture piscine
      Relay2 = RELAY_OPEN;
      digitalWrite(pRelay2, Relay2);
      return true;

    case OPEN_CMD_ACTIVATED :
      // fermer le Relay2 => Ouverture piscine
      Relay2 = RELAY_CLOSED;
      digitalWrite(pRelay2, Relay2);
      return true;

    default:
      // erreur
      return false;
  }
}
