int DomoticzCycle = 1;

//*****************************************
//  send data over USB
//*****************************************
void SendDataOverUSB ()
{
  printlnD();
  printlnD("----------------------------------");
  printlnD("*** TEMPERATURES ***");
  if (PoolState.ErrorTempSensorInit) {
    printD("Error temp = ");
    printlnD(PoolState.ErrorTempSensorInit);
  };
  if (PoolState.ErrorTempSensorInit0) {
    printD("Error temp device 0 = ");
    printlnD(PoolState.ErrorTempSensorInit0);
  };
  if (PoolState.ErrorTempSensorInit1) {
    printD("Error temp device 1 = ");
    printlnD(PoolState.ErrorTempSensorInit1);
  };
  printD("Air       = ");
  printD(PoolState.AirTemp);
  printlnD(" °C");
  printD("Eau       = ");
  printD(PoolState.WaterTemp);
  printlnD(" °C");

  printlnD("*** ETAT ***");
  printD("Mode commandé par la clé : ");
  if (Automat_Mode.ModeState == MANUAL) {
    printlnD("MANUEL");
  }
  else {
    printlnD("AUTOMATIQUE");
  }
  if (Automat_Mode.ErrorMode) printlnD("ERREUR de MODE");
  printD("Volet : DERNIERE COMMANDE = ");
  switch (Automat_Cmd.CommandState) {
    case CLOSE_CMD_ACTIVATED:
      printlnD("FERMETURE");
      break;
    case OPEN_CMD_ACTIVATED:
      printlnD("OUVERTURE");
      break;
    default:
      // statements
      printlnD("INDEFINI");
      break;
  }
  if (Automat_Cmd.ErrorCmd)
    printlnD("ERREUR de CMD");
  printlnD();

  printlnD("*** PARAMETER ***");
  printD("Temperature Air = Temperature Eau +");
  printD(String(Seuil()));
  printD(" (ouverture) / ");
  printD(String(Seuil() - Hysteresis()));
  printD(" (fermeture)");
  printlnD();

  printlnV("*** DEBUG ***");
  printV("Entré Switch = "); printV(AutoSwitch == HIGH ? "HIGH / OUVERT / MANU" : "LOW / FERME / AUTO");
  printlnV();
  printV("Sortie LED auto = "); printV(AutoLED);
  printlnV();
  printV("Sortie LED temp = R="); printV(TempLEDred); printD(" / V="); printV(TempLEDgreen); printV(" / B="); printV(TempLEDblue);
  printlnV();
  printV("Sortie Relai 1  = "); printV(Relay1 == HIGH ? "HIGH / REPOS => La clé manuelle a le controle" : "LOW / ACTIF => L'automate a le controle");
  printlnV();
  printV("Sortie Relai 2  = "); printV(Relay2 == HIGH ? "HIGH / REPOS => Volet fermé" : "LOW / ACTIF => Volet ouvert");
  printlnV();
}

//*****************************************
//  send data over WiFi
//*****************************************
void SendDataToDomoticz ()
{
  printlnV();
  printV("Wi-Fi DomoticzCycle = "); printlnV(DomoticzCycle);

  switch (DomoticzCycle) {
    case 1:
      //----------------------------------------
      // Transmettre Temperature Air à Domoticz
      //----------------------------------------
      if (!PoolState.ErrorTempSensorInit) {
        sendSvalueDomoticz(String(PoolState.AirTemp), Configuration.domoticz.idxs.idx_airTemp);
        printV(">> TempAir = "); printlnV(PoolState.AirTemp);
      }
      ++DomoticzCycle;
      break;

    case 2:
      //----------------------------------------
      // Transmettre Temperature Eau à Domoticz
      //----------------------------------------
      if (!PoolState.ErrorTempSensorInit) {
        sendSvalueDomoticz(String(PoolState.WaterTemp), Configuration.domoticz.idxs.idx_waterTemp);
        printV(F(">> TempEau = ")); printlnV(PoolState.WaterTemp);
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
      if (Automat_Mode.ModeState == MANUAL) {
        printlnV(F(">> Automat = Off"));
      }
      else {
        printlnV(F(">> Automat = On"));
      }
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
      switch (Automat_Cmd.CommandState) {
        case CLOSE_CMD_ACTIVATED:
          printlnV(F(">> Volet = Fermeture"));
          break;
        case OPEN_CMD_ACTIVATED:
          printlnV(F(">> Volet = Ouverture"));
          break;
        default:
          printlnV(F(">> Volet = Indefini"));
          break;
      }
      ++DomoticzCycle;
      break;

    default:
      DomoticzCycle = 1;
      break;
  }
}
