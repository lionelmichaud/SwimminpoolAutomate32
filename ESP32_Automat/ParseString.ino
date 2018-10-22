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
      sendSvalueDomoticz(String(PoolState.AirTemp), String(idx_airTemp));
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
      sendSvalueDomoticz(String(PoolState.WaterTemp), String(idx_waterTemp));
    }
  }

  //--------------------------------------
  // TEMPERATURE INTERNE BOITIER MAXIMUM
  //--------------------------------------
  else if (receivedString.startsWith("TMaxInt= ")) {
    localFloat = receivedString.substring(9).toFloat();
#if defined ECHO
    Serial.print(">TempMaxInt = "); Serial.println(localFloat);
#endif
    if ((localFloat > 0.0) && (localFloat < 150.0)) PoolState.MaxIntTemp = localFloat;
  }

  //-------------------------------------
  // TEMPERATURE INTERNE BOITIER
  //-------------------------------------
  else if (receivedString.startsWith("T Int= ")) {
    localFloat = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">TempInt  = "); Serial.println(localFloat);
#endif
    if ((localFloat > 0.0) && (localFloat < 150.0)) PoolState.IntTemp = localFloat;
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
      PoolState.AutomateString = "On";
      sendSwitchCmdDomoticz(PoolState.AutomateString, String(idx_automate));

    } else if (localString.startsWith("Off")) {
      PoolState.AutomateString = "Off";
      sendSwitchCmdDomoticz(PoolState.AutomateString, String(idx_automate));

    } else PoolState.AutomateString = "";
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
      PoolState.VoletString = "Ouverture";
      sendSwitchCmdDomoticz("Off", String(idx_posVolet));

    } else if (localString.startsWith("Fermeture")) {
      PoolState.VoletString = "Fermeture";
      sendSwitchCmdDomoticz("On", String(idx_posVolet));

    } else PoolState.VoletString = "";
  }

  //--------------------------------
  // SEUILS D'OUVERTURE/FERMETURE
  //--------------------------------
  else if (receivedString.startsWith("Seuil: ")) {
    AutomatParam.Seuil = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Seuil = "); Serial.println(AutomatParam.Seuil);
#endif
  }

  //------------------------------------
  // HYSTERESIS DE L'OUVERTURE/FERMETURE
  //------------------------------------
  else if (receivedString.startsWith("Hyste: ")) {
    AutomatParam.Hysteresis = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Hyste = "); Serial.println(AutomatParam.Hysteresis);
#endif
  }

  //------------------------------------
  // OFFSET DE MESURE DE TEMPERATURE EAU
  //------------------------------------
  else if (receivedString.startsWith("d Eau: ")) {
    AutomatParam.OffsetEau = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Offset Eau = "); Serial.println(AutomatParam.OffsetEau);
#endif
  }

  //------------------------------------
  // OFFSET DE MESURE DE TEMPERATURE AIR
  //------------------------------------
  else if (receivedString.startsWith("d Air: ")) {
    AutomatParam.OffsetAir = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Offset Air = "); Serial.println(AutomatParam.OffsetAir);
#endif
  }
}
