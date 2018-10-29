//-------------------------------------------------
//  blink Red Auto LED when in AUTOMATIC Mode
//-------------------------------------------------
void BlinkRedAutoLED ()
{
  if (!PoolState.ErrorTemp)
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
//  Relais au repos = HIGH
//-------------------------------------------------
boolean SwitchRelayAutoManu (int State) {
  switch (State) {

    case MANUAL :
      // ouvrir le Relay1 pour etre sur de se retrouver en command manuelle
      Relay1 = HIGH; //  Relais au repos
      digitalWrite(pRelay1, Relay1);
      return true;

    case AUTOMATIC :
      // fermer le Relay1 pour passer en automatique
      Relay1 = LOW;
      digitalWrite(pRelay1, Relay1);
      return true;

    default:
      // erreur
      return false;
  }
}

//-------------------------------------------------
// MANAGE RELAY : OPEN || CLOSE
//  Relais au repos = HIGH
//-------------------------------------------------
boolean SwitchRelayOpenCloseCover (int State) {
  switch (State) {

    case CLOSE_CMD_ACTIVATED :
      // ouvrir le Relay2 => Fermeture piscine
      Relay2 = HIGH; //  Relais au repos
      digitalWrite(pRelay2, Relay2);
      return true;

    case OPEN_CMD_ACTIVATED :
      // fermer le Relay2 => Ouverture piscine
      Relay2 = LOW;
      digitalWrite(pRelay2, Relay2);
      return true;

    default:
      // erreur
      return false;
  }
}
