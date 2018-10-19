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
      AirTemp = localFloat;
      sendSvalueDomoticz(String(AirTemp), String(idx_airTemp));
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
      WaterTemp = localFloat;
      sendSvalueDomoticz(String(WaterTemp), String(idx_waterTemp));
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
    if ((localFloat > 0.0) && (localFloat < 150.0)) MaxIntTemp = localFloat;
  }

  //-------------------------------------
  // TEMPERATURE INTERNE BOITIER
  //-------------------------------------
  else if (receivedString.startsWith("T Int= ")) {
    localFloat = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">TempInt  = "); Serial.println(localFloat);
#endif
    if ((localFloat > 0.0) && (localFloat < 150.0)) IntTemp = localFloat;
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
      AutomateString = "On";
      sendSwitchCmdDomoticz(AutomateString, String(idx_automate));

    } else if (localString.startsWith("Off")) {
      AutomateString = "Off";
      sendSwitchCmdDomoticz(AutomateString, String(idx_automate));

    } else AutomateString = "";
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
      VoletString = "Ouverture";
      sendSwitchCmdDomoticz("Off", String(idx_posVolet));

    } else if (localString.startsWith("Fermeture")) {
      VoletString = "Fermeture";
      sendSwitchCmdDomoticz("On", String(idx_posVolet));

    } else VoletString = "";
  }

  //--------------------------------
  // SEUILS D'OUVERTURE/FERMETURE
  //--------------------------------
  else if (receivedString.startsWith("Seuil: ")) {
    Seuil = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Seuil = "); Serial.println(Seuil);
#endif
  }

  //------------------------------------
  // HYSTERESIS DE L'OUVERTURE/FERMETURE
  //------------------------------------
  else if (receivedString.startsWith("Hyste: ")) {
    Hysteresis = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Hyste = "); Serial.println(Hysteresis);
#endif
  }

  //------------------------------------
  // OFFSET DE MESURE DE TEMPERATURE EAU
  //------------------------------------
  else if (receivedString.startsWith("d Eau: ")) {
    OffsetEau = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Offset Eau = "); Serial.println(OffsetEau);
#endif
  }

  //------------------------------------
  // OFFSET DE MESURE DE TEMPERATURE AIR
  //------------------------------------
  else if (receivedString.startsWith("d Air: ")) {
    OffsetAir = receivedString.substring(7).toFloat();
#if defined ECHO
    Serial.print(">Offset Air = "); Serial.println(OffsetAir);
#endif
  }
}
