int DomoticzCycle = 1;

//*****************************************
//  send data over USB
//*****************************************
void SendDataOverUSB ()
{
  Serial.println();
  Serial.println(F("----------------------------------"));
  Serial.println(F("*** TEMPERATURES ***"));
  if (PoolState.ErrorTempSensorInit) {
    Serial.print(F("Error temp = "));
    Serial.println(PoolState.ErrorTempSensorInit);
  };
  if (PoolState.ErrorTempSensorInit0) {
    Serial.print(F("Error temp device 0 = "));
    Serial.println(PoolState.ErrorTempSensorInit0);
  };
  if (PoolState.ErrorTempSensorInit1) {
    Serial.print(F("Error temp device 1 = "));
    Serial.println(PoolState.ErrorTempSensorInit1);
  };
  Serial.print(F("Air       = "));
  Serial.print(PoolState.AirTemp);
  Serial.println(" °C");
  Serial.print(F("Eau       = "));
  Serial.print(PoolState.WaterTemp);
  Serial.println(" °C");

  Serial.println(F("*** ETAT ***"));
  Serial.print(F("Mode commandé par la clé : "));
  if (Automat_Mode.ModeState == MANUAL)
    Serial.println(F("MANUEL"));
  else
    Serial.println(F("AUTOMATIQUE"));
  if (Automat_Mode.ErrorMode)
    Serial.println(F("ERREUR de MODE"));
  Serial.print(F("Volet : DERNIERE COMMANDE = "));
  switch (Automat_Cmd.CommandState) {
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
  if (Automat_Cmd.ErrorCmd)
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
  Serial.print(F("Sortie LED temp = R="));
  Serial.print(TempLEDred); Serial.print(" / V="); Serial.print(TempLEDgreen); Serial.print(" / B="); Serial.print(TempLEDblue);
  Serial.println();
  Serial.print(F("Sortie Relai 1  = "));
  Serial.print(Relay1 == HIGH ? "HIGH / REPOS => La clé manuelle a le controle" : "LOW / ACTIF => L'automate a le controle");
  Serial.println();
  Serial.print(F("Sortie Relai 2  = "));
  Serial.print(Relay2 == HIGH ? "HIGH / REPOS => Volet fermé" : "LOW / ACTIF => Volet ouvert");
  Serial.println();
#endif
}

//*****************************************
//  send data over WiFi
//*****************************************
void SendDataToDomoticz ()
{
#if defined DEBUG
  Serial.println(); Serial.print("Wi-Fi DomoticzCycle = "); Serial.println(DomoticzCycle);
#endif

  switch (DomoticzCycle) {
    case 1:
      //----------------------------------------
      // Transmettre Temperature Air à Domoticz
      //----------------------------------------
      if (!PoolState.ErrorTempSensorInit) {
        sendSvalueDomoticz(String(PoolState.AirTemp), Configuration.domoticz.idxs.idx_airTemp);
#if defined DEBUG
        Serial.print(F(">> TempAir = "));
        Serial.println(PoolState.AirTemp);
#endif
      }
      ++DomoticzCycle;
      break;

    case 2:
      //----------------------------------------
      // Transmettre Temperature Eau à Domoticz
      //----------------------------------------
      if (!PoolState.ErrorTempSensorInit) {
        sendSvalueDomoticz(String(PoolState.WaterTemp), Configuration.domoticz.idxs.idx_waterTemp);
#if defined DEBUG
        Serial.print(F(">> TempEau = "));
        Serial.println(PoolState.WaterTemp);
#endif
      }
      ++DomoticzCycle;
      break;

    case 3:
      //------------------------------------------------
      // Transmettre Mode de fonctionnement à Domoticz
      //------------------------------------------------
      switch (Automat_Mode.ModeState) {
        case MANUAL:
          sendSwitchCmdDomoticz("Off", Configuration.domoticz.idxs.idx_automate);
          break;
        case AUTOMATIC:
          sendSwitchCmdDomoticz("On", Configuration.domoticz.idxs.idx_automate);
          break;
        default:
          break;
      }
#if defined DEBUG
      if (Automat_Mode.ModeState == MANUAL)
        Serial.println(F(">> Automat = Off"));
      else
        Serial.println(F(">> Automat = On"));
#endif
      ++DomoticzCycle;
      break;

    case 4:
      //------------------------------------------
      // Transmettre Position du volet à Domoticz
      //------------------------------------------
      switch (Automat_Cmd.CommandState) {
        case CLOSE_CMD_ACTIVATED:
          sendSwitchCmdDomoticz("On", Configuration.domoticz.idxs.idx_posVolet);
          break;
        case OPEN_CMD_ACTIVATED:
          sendSwitchCmdDomoticz("Off", Configuration.domoticz.idxs.idx_posVolet);
          break;
        default:
          break;
      }
#if defined DEBUG
      switch (Automat_Cmd.CommandState) {
        case CLOSE_CMD_ACTIVATED:
          Serial.println(F(">> Volet = Fermeture"));
          break;
        case OPEN_CMD_ACTIVATED:
          Serial.println(F(">> Volet = Ouverture"));
          break;
        default:
          Serial.println(F(">> Volet = Indefini"));
          break;
      }
#endif
      ++DomoticzCycle;
      break;

    default:
      DomoticzCycle = 1;
      break;
  }
}
