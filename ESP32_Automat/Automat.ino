//------------------------------------------------------------------------------------------------
String CurrentModeString() {
  switch (Automat_Mode.ModeState) {
    case MANUAL:
      return "MANUEL";
    case AUTOMATIC:
      return "AUTOMATIQUE";
    default:
      return "INCONNU";
  }
}

//------------------------------------------------------------------------------------------------
String CurrentModeDomoticz() {
  switch (Automat_Mode.ModeState) {
    case MANUAL:
      return "Off";
    case AUTOMATIC:
      return "On";
    default:
      return "";
  }
}

//------------------------------------------------------------------------------------------------
String CurrentCoverPositionString() {
  switch (Automat_Cmd.CommandState) {
    case CLOSE_CMD_ACTIVATED:
      return "FERME";
    case OPEN_CMD_ACTIVATED:
      return "OUVERT";
    default:
      return "INCONNU";
  }
}

//------------------------------------------------------------------------------------------------
String CurrentCoverPositionDomoticz() {
  switch (Automat_Cmd.CommandState) {
    case CLOSE_CMD_ACTIVATED:
      return "On";
    case OPEN_CMD_ACTIVATED:
      return "Off";
    default:
      return "";
  }
}

//------------------------------------------------------------------------------------------------
boolean LongPeriodOfLowAirTemp ()
{
  return (PeriodOfLowAirTemp >= NbPeriodCold());
}

//------------------------------------------------------------------------------------------------
//  Measure how long the air temp is lower than water temp
//------------------------------------------------------------------------------------------------
void MeasurePeriodOfCold()
{
#if defined DEBUG
  Serial.println("-------------------------------- -TIMER CALLBACK------------------------------ -");
  Serial.print(F("Millis = "));    Serial.println(currentMillis);
#endif
  if (PoolState.AirTemp < (PoolState.WaterTemp + Seuil() - Hysteresis()))
  {
    PeriodOfLowAirTemp = PeriodOfLowAirTemp + 1;
#if defined DEBUG
    Serial.print(F("Duration = "));    Serial.println(PeriodOfLowAirTemp);
#endif
  }
  else
    PeriodOfLowAirTemp = 0;
}

//------------------------------------------------------------------------------------------------
// MANAGE STATE MACHINES
//------------------------------------------------------------------------------------------------
void AutomatRun(Automat_Mode_T theAutomatMode, Automat_Cmd_T theAutomatCmd, int theSwitchState) {
  switch (theAutomatMode.ModeState) {

    case MANUAL:
      //------------------
      // En Mode MANUAL
      //------------------
      switch (theSwitchState) {
        case AUTOMATIC :
          //-------------------------------------
          // Switch commuté en position AUTOMATIC
          //-------------------------------------
          // commutation en mode Automatic
          theAutomatMode.ModeState = AUTOMATIC;

          // Allumer la LED auto
          AutoLED = HIGH;
          digitalWrite(pAutoLED, AutoLED);
          break;

        case MANUAL :
          //-----------------------------------
          // Switch commuté en position MANUAL
          //-----------------------------------
          theAutomatMode.prevModeState = theAutomatMode.ModeState;
          break;
      }

#if defined CLOSURE_TEMPO
      timer.disable(TimerColdID); // désactiver le timer en attendant d'en avoir besoin
#endif

      theAutomatMode.ErrorMode = false;
      break;

    case AUTOMATIC:
      //--------------------
      //  En Mode AUTOMATIC
      //--------------------
      switch (theSwitchState) {
        case MANUAL :
          //-----------------------------------
          // Switch commuté en position MANUAL
          //-----------------------------------
          // commuter en mode Manual
          theAutomatMode.ModeState = MANUAL;

          // eteindre la LED auto
          AutoLED = LOW;
          digitalWrite(pAutoLED, AutoLED);

          // ouvrir le Relay1 pour etre sur de se retrouver en command manuelle
          SwitchRelayAutoManu (MANUAL);
          break;

        case AUTOMATIC :
          //-------------------------------------
          // Switch commuté en position AUTOMATIC
          //-------------------------------------
          // rester en mode Automatic
          if (theAutomatMode.prevModeState == MANUAL) {
            //---------------------------------------------------------------------------------------------
            // on vient d'entrer dans le mode Automatic => il faut choisir entre cmd Ouverture ou Fermeture
            //---------------------------------------------------------------------------------------------
            prevMillis = millis();
            if (PoolState.AirTemp > PoolState.WaterTemp + Seuil())
              // commutation en mode ouverture piscine
              theAutomatCmd.CommandState = OPEN_CMD_ACTIVATED;
            else
              // commutation en mode fermeture piscine
              theAutomatCmd.CommandState = CLOSE_CMD_ACTIVATED;
          }

          //---------------------------------------------------
          // Gérer l'ouverture et la fermeture du volet roulant
          //---------------------------------------------------
          switch (theAutomatCmd.CommandState) {
            case CLOSE_CMD_ACTIVATED:
              //------------------------------
              // Commande de fermeture active
              //------------------------------
              currentMillis = millis();
              if (currentMillis - prevMillis <= timeoutOpenClose) {
                //----------------------------------------------------------------------
                // La commande de fermeture auto est activée pendant une certaine durée
                //----------------------------------------------------------------------
                // ouvrir le Relay2 => Fermeture piscine
                SwitchRelayOpenCloseCover(CLOSE_CMD_ACTIVATED);

                // fermer le Relay1 pour passer en automatique
                SwitchRelayAutoManu (AUTOMATIC);
              }
              else {
                //------------------------------------------------------------------------
                // Le volet est maintenant fermé => hand-over control to the manual switch
                //------------------------------------------------------------------------
                // ouvrir le Relay1 pour rendre la main au controle manuel
                SwitchRelayAutoManu (MANUAL);
              };

              if (PoolState.AirTemp > PoolState.WaterTemp + Seuil())
              {
                // Conditions favorables => commutation en mode ouverture piscine
                theAutomatCmd.CommandState = OPEN_CMD_ACTIVATED;
                prevMillis = millis();
              }
              else
                theAutomatCmd.prevCommandState = theAutomatCmd.CommandState;

              theAutomatCmd.ErrorCmd = false;
              break;

            case OPEN_CMD_ACTIVATED:
              //------------------------------
              // Commande d'ouverture active
              //------------------------------
              currentMillis = millis();
              if (currentMillis - prevMillis <= timeoutOpenClose) {
                //----------------------------------------------------------------------
                // La commande d'ouverture auto est activée pendant une certaine durée
                //----------------------------------------------------------------------
                // fermer le Relay2 => Ouverture piscine
                SwitchRelayOpenCloseCover(OPEN_CMD_ACTIVATED);

                // fermer le Relay1 pour passer en automatique
                SwitchRelayAutoManu (AUTOMATIC);

                PeriodOfLowAirTemp = 0; // Initilize counter of low air temp
              }
              else {
                //-------------------------------------------------------------------------
                // Le volet est maintenant ouvert => hand-over control to the manual switch
                //-------------------------------------------------------------------------
                // ouvrir le Relay1 pour repasser en manuel
                SwitchRelayAutoManu (MANUAL);

#if defined CLOSURE_TEMPO
                if (! timer.isEnabled(TimerColdID)) {
                  // PeriodOfLowAirTemp = 0; // Initilize counter of low air temp
                  timer.restartTimer(TimerColdID); // reset T0 du timer périodique
                  timer.enable(TimerColdID); // activer le timer périodique
#if defined DEBUG
                  Serial.println("---------------------------- -TIMER ENABLED AND STARTED !-------------------------- -");
                  Serial.print(F("Millis = "));    Serial.println(currentMillis);
#endif
                }
#endif
              };

              //------------------------------------------------------------------------------------------
              // Fermer si la température de l'air baisse longtemps en dessous de la température de l'eau
              //------------------------------------------------------------------------------------------
#if defined CLOSURE_TEMPO
              if (LongPeriodOfLowAirTemp())
              {
                // Conditions défavorables => commutation en mode fermeture piscine
                theAutomatCmd.CommandState = CLOSE_CMD_ACTIVATED;
                prevMillis = millis();
                timer.disable(TimerColdID); // désactiver le timer périodique
#if defined DEBUG
                Serial.println("-------------------------------- -TIMER DISABLED !------------------------------ -");
#endif
              }
#else
              if (PoolState.AirTemp < (PoolState.WaterTemp + Seuil() - Hysteresis()))
              {
                // commutation en mode fermeture piscine
                theAutomatCmd.CommandState = CLOSE_CMD_ACTIVATED;
                prevMillis = millis();
              }
#endif
              else
                theAutomatCmd.prevCommandState = theAutomatCmd.CommandState;

              theAutomatCmd.ErrorCmd = false;
              break;

            default:
              // erreur
              theAutomatCmd.ErrorCmd = true;
              break;
          }

          theAutomatMode.prevModeState = theAutomatMode.ModeState;
          break;
      }

      theAutomatMode.ErrorMode = false;
      break;

    default:
      // erreur
      theAutomatMode.ErrorMode = true;
      break;
  }
}
