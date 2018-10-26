//*****************************************
//  send data over USB
//*****************************************
void SendDataUSB ()
{
  Serial.println();
  Serial.println(F("----------------------------------"));
  Serial.println(F("*** TEMPERATURES ***"));
  if (ErrorTemp) {
    Serial.print(F("Error temp = "));
    Serial.println(ErrorTemp);
  };
  if (ErrorTemp0) {
    Serial.print(F("Error temp device 0 = "));
    Serial.println(ErrorTemp0);
  };
  if (ErrorTemp1) {
    Serial.print(F("Error temp device 1 = "));
    Serial.println(ErrorTemp1);
  };
  Serial.print(F("Air       = "));
  Serial.print(PoolState.AirTemp);
  Serial.println(" °C");
  Serial.print(F("Eau       = "));
  Serial.print(PoolState.WaterTemp);
  Serial.println(" °C");

  Serial.println(F("*** ETAT ***"));
  Serial.print(F("Mode commandé par la clé : "));
  if (Automat_Mode_state.ModeState == MANUAL)
    Serial.println(F("MANUEL"));
  else
    Serial.println(F("AUTOMATIQUE"));
  if (ErrorMode)
    Serial.println(F("ERREUR de MODE"));
  Serial.print(F("Volet : DERNIERE COMMANDE = "));
  switch (Automat_2_state.CommandState) {
    case CLOSE_CMD_ACTIVATED:
      Serial.println(F("FERMETURE"));
      break;
    case OPEN_CMD_ACTIVATED:
      Serial.println(F("OUVERTURE"));
      break;
    default:
      // statements
      Serial.println(F("INDEFINI"));
      break;
  }
  if (ErrorCmd)
    Serial.println(F("ERREUR de CMD"));
  Serial.println();

  Serial.println(F("*** PARAMETER ***"));
  Serial.print(F("Temperature Air = Temperature Eau +"));
  Serial.print(String(Seuil()));
  Serial.print(F(" (ouverture) / "));
  Serial.print(String(Seuil() - Hysteresis()));
  Serial.print(F(" (fermeture)"));
  Serial.println();

#if defined DEBUG
  Serial.println(F("*** DEBUG ***"));
  Serial.print(F("Entré Switch = "));
  Serial.print(AutoSwitch == HIGH ? "HIGH / OUVERT / MANU" : "LOW / FERME / AUTO");
  Serial.println();
  Serial.print(F("Sortie LED auto = "));
  Serial.print(AutoLED);
  Serial.println();
  Serial.print(F("Sortie LED temp = "));
  Serial.print(TempLEDred); Serial.print(" / "); Serial.print(TempLEDgreen); Serial.print(" / "); Serial.print(TempLEDblue);
  Serial.println();
  Serial.print(F("Sortie Relai 1  = "));
  Serial.print(Relay1 == HIGH ? "REPOS => La clé manuelle à la main" : "ACTIF => L'automate à la main");
  Serial.println();
  Serial.print(F("Sortie Relai 2  = "));
  Serial.print(Relay2 == HIGH ? "REPOS => Volet fermé" : "ACTIF => Volet ouvert");
  Serial.println();
#endif
}
