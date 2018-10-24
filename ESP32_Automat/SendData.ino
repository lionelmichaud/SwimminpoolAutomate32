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
  if (Automat_1_state.ModeState == MANUAL)
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

//*****************************************
//  send data over WiFi
//*****************************************
#if defined WIFI_OUTPUT
void SendDataWiFi ()
{
#if defined DEBUG
  Serial.println(); Serial.print("WiFiserialCycle = "); Serial.println(WiFiserialCycle);
#endif

  switch (WiFiserialCycle) {
    case 1:
      if (!ErrorTemp) {
        WiFiserial.print(F("T Air= "));
        WiFiserial.println(AirTemp);
#if defined DEBUG
        Serial.print(F("<TempAir = "));
        Serial.println(AirTemp);
#endif
        ++WiFiserialCycle;
      }
      break;

    case 2:
      if (!ErrorTemp) {
        WiFiserial.print(F("T Eau= "));
        WiFiserial.println(WaterTemp);
#if defined DEBUG
        Serial.print(F("<TempEau = "));
        Serial.println(WaterTemp);
#endif
        ++WiFiserialCycle;
      }
      break;

    case 3:
      WiFiserial.print(F("T Int= "));
      WiFiserial.println(IntTempR);
#if defined DEBUG
      Serial.print(F("<TempInt = "));
      Serial.println(IntTempR);
#endif
      ++WiFiserialCycle;
      break;

    case 4:
#if defined EEPROM_OUTPUT
      WiFiserial.print(F("TMaxInt= "));
      WiFiserial.println(GetMaxIntTemp());
#if defined DEBUG
      Serial.print(F("<TMaxInt = "));
      Serial.println(EEPROM_IntTempR);
#endif
#endif
      ++WiFiserialCycle;
      break;

    case 5:
      if (ModeState == MANUAL)
        WiFiserial.println(F("Auto= Off"));
      else
        WiFiserial.println(F("Auto= On"));

#if defined DEBUG
      if (ModeState == MANUAL)
        Serial.println(F("<Automat = Off"));
      else
        Serial.println(F("<Automat = On"));
#endif
      ++WiFiserialCycle;
      break;

    case 6:
      switch (CommandState) {
        case CLOSE_CMD_ACTIVATED:
          WiFiserial.println(F("Volet= Fermeture"));
          break;
        case OPEN_CMD_ACTIVATED:
          WiFiserial.println(F("Volet= Ouverture"));
          break;
        default:
          // statements
          WiFiserial.println(F("Volet= Indefini"));
          break;
      }
#if defined DEBUG
      switch (CommandState) {
        case CLOSE_CMD_ACTIVATED:
          Serial.println(F("<Volet = Fermeture"));
          break;
        case OPEN_CMD_ACTIVATED:
          Serial.println(F("<Volet = Ouverture"));
          break;
        default:
          // statements
          Serial.println(F("<Volet = Indefini"));
          break;
      }
#endif
      ++WiFiserialCycle;
      break;

    case 7:
      // Transmission à l'ESP de la version logicielle
      WiFiserial.print(F("Log: "));
      WiFiserial.print(SOFTWARE); WiFiserial.print("-");
      WiFiserial.println(VERSION);

      ++WiFiserialCycle;
      break;

    case 8:
      // Transmission à l'ESP des seuils d'ouverture/fermeture
      WiFiserial.print(F("Seuil: "));
      WiFiserial.println(TEMPOFFSETTHRESHOLD);

      ++WiFiserialCycle;
      break;

    case 9:
      // Transmission à l'ESP de l'offset de mesure de la température de l'air
      WiFiserial.print(F("d Air: "));
      WiFiserial.println(GetAirTempOffset());

      ++WiFiserialCycle;
      break;

    case 10:
      // Transmission à l'ESP de l'offset de mesure de la température de l'air
      WiFiserial.print(F("d Eau: "));
      WiFiserial.println(GetWaterTempOffset());

      ++WiFiserialCycle;
      break;

    default:
      // Transmission à l'ESP de l'hysteresis d'ouverture/fermeture
      WiFiserial.print(F("Hyste: "));
      WiFiserial.println(TrimDelta);

      WiFiserialCycle = 1;
      break;
  }

  if (ErrorCmd)
    WiFiserial.println(F("<ERREUR de CMD "));

  if (WiFiserial.overflow())
    Serial.println("!!SoftwareSerial overflow during Tx!!");
}
#endif
