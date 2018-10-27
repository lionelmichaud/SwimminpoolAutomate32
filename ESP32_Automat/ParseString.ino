//--------------------------------------------------------------------
// DECODAGE DE LA CHAINE DE CHARACTERES RECUE
//--------------------------------------------------------------------
void parseString(String receivedString) {
  String localString = "";
  float localFloat = 0.0;

  //-------------------------
  // TEMPERATURE DE L'AIR
  //-------------------------
  if (receivedString.startsWith("T Air= ")) {
    localFloat = receivedString.substring(7).toFloat();

#if defined ECHO
    Serial.print(F(">TempAir = ")); Serial.println(localFloat);
#endif

    if ((localFloat > 0.0) && (localFloat < 60.0)) {
      PoolState.AirTemp = localFloat;
      //      sendSvalueDomoticz(String(PoolState.AirTemp), String(idx_airTemp));
    }
  }

  //-------------------------
  // TEMPERATURE DE L'EAU
  //-------------------------
  else if (receivedString.startsWith("T Eau= ")) {
    localFloat = receivedString.substring(7).toFloat();

#if defined ECHO
    Serial.print(F(">TempEau = ")); Serial.println(localFloat);
#endif

    if ((localFloat > 0.0) && (localFloat < 40.0)) {
      PoolState.WaterTemp = localFloat;
      //      sendSvalueDomoticz(String(PoolState.WaterTemp), String(idx_waterTemp));
    }
  }

  //--------------------------------------
  // MODE DE FONCTIONNEMENT DE L'AUTOMATE
  //--------------------------------------
  else if (receivedString.startsWith("Auto= ")) {
    localString = receivedString.substring(6);
    int n = localString.length() - 1;
    localString.remove(n);
#if defined ECHO
    Serial.print(">Automate = "); Serial.println(localString);
#endif
    if (localString.startsWith("On")) {
      Automat_Mode.ModeState = AUTOMATIC;
      //      sendSwitchCmdDomoticz(PoolState.AutomateString, String(idx_automate));

    } else if (localString.startsWith("Off")) {
      Automat_Mode.ModeState = MANUAL;
      //      sendSwitchCmdDomoticz(PoolState.AutomateString, String(idx_automate));

    } else Automat_Mode.ModeState = UNDEF_MODE;
  }

  //---------------------------
  // POSITION DU VOLET ROULANT
  //---------------------------
  else if (receivedString.startsWith("Volet=")) {
    localString = receivedString.substring(6);
    int n = localString.length() - 1;
    localString.remove(n);
#if defined ECHO
    Serial.print(">Volet = "); Serial.println(localString);
#endif
    if (localString.startsWith("Ouverture")) {
      Automat_Cmd.CommandState = OPEN_CMD_ACTIVATED;
      //      sendSwitchCmdDomoticz("Off", String(idx_posVolet));

    } else if (localString.startsWith("Fermeture")) {
      Automat_Cmd.CommandState = CLOSE_CMD_ACTIVATED;
      //      sendSwitchCmdDomoticz("On", String(idx_posVolet));

    } else Automat_Cmd.CommandState = UNDEF_CMD;
  }

  //--------------------------------
  // SEUILS D'OUVERTURE/FERMETURE
  //--------------------------------
  else if (receivedString.startsWith("Seuil: ")) {
    SetSeuil(receivedString.substring(7).toFloat());
#if defined ECHO
    Serial.print(">Seuil = "); Serial.println(Seuil());
#endif
  }

  //------------------------------------
  // HYSTERESIS DE L'OUVERTURE/FERMETURE
  //------------------------------------
  else if (receivedString.startsWith("Hyste: ")) {
    SetHysteresis(receivedString.substring(7).toFloat());
#if defined ECHO
    Serial.print(">Hyste = "); Serial.println(Hysteresis());
#endif
  }

  //------------------------------------
  // OFFSET DE MESURE DE TEMPERATURE EAU
  //------------------------------------
  else if (receivedString.startsWith("d Eau: ")) {
    SetWaterTempOffset(receivedString.substring(7).toFloat());
#if defined ECHO
    Serial.print(">Offset Eau = "); Serial.println(WaterTempOffset());
#endif
  }

  //------------------------------------
  // OFFSET DE MESURE DE TEMPERATURE AIR
  //------------------------------------
  else if (receivedString.startsWith("d Air: ")) {
    SetAirTempOffset(receivedString.substring(7).toFloat());
#if defined ECHO
    Serial.print(">Offset Air = "); Serial.println(AirTempOffset());
#endif
  }
}
