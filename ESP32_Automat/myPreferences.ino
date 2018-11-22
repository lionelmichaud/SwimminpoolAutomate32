// Paramètres de fonctionnement de l'automate
//  Nombre de périodes 'periodColdTimer' pendant lesquelles la température
//    à été vue basse avant de femer le volet (protection contre le passage de nuage)
//  Offset de température pour ouvrir la piscine
//  - Ouverture volet si (AirTemp > WaterTemp + Seuil)
//  - Fermeture volet si (AirTemp < WaterTemp + Seuil - Hysteresis)
//
//  bool Summer         = 1;   // Heure d'été
//  int NbPeriodCold    = 15;  // NbPeriodCold * periodColdTimer = durée en milisecondes
//  float Seuil         = 1.0; // Ouverture volet si (AirTemp > WaterTemp + Seuil)
//  float Hysteresis    = 2.0; // Fermeture volet si (AirTemp < WaterTemp + Seuil - Hysteresis)
//  float OffsetAir     = 0.0;
//  float OffsetEau     = 0.0;

//------------------------------------------------
// INCREMENT WATER TEMP OFFSET IN EEPROM MEMORY
//------------------------------------------------
void SetWaterTempOffset(float OffsetIncrement) {
  float TempOffset = 0.0;
  printD(F("Incrément d'offset Température Eau = ")); printlnD(OffsetIncrement);

  // retreive the stored offset in preferences
  TempOffset = preferences.getFloat("WaterOffset", 0.0);

  // increment
  TempOffset = TempOffset + OffsetIncrement;

  // store the offset in preferences
  preferences.putFloat("WaterOffset", TempOffset);

  DumpPreferences();
}

//------------------------------------------------
// INCREMENT AIR TEMP OFFSET IN EEPROM MEMORY
//------------------------------------------------
void SetAirTempOffset(float OffsetIncrement) {
  float TempOffset = 0.0;
  printD(F("Incrément d'offset Température Air = ")); printlnD(OffsetIncrement);

  // retreive the stored offset in preferences
  TempOffset = preferences.getFloat("AirOffset", 0.0);

  // increment
  TempOffset = TempOffset + OffsetIncrement;

  // store the offset in preferences
  preferences.putFloat("AirOffset", TempOffset);

  DumpPreferences();
}

//------------------------------------------------
void SetSeuil(float Seuil) {
  preferences.putFloat("Seuil", Seuil);
}

//------------------------------------------------
void SetHysteresis(float Hysteresis) {
  preferences.putFloat("Hysteresis", Hysteresis);
}

//------------------------------------------------
int NbPeriodCold() {
  return preferences.getInt("NbPeriodCold", 15);
}

//------------------------------------------------
float Seuil() {
  return preferences.getFloat("Seuil", 1.0);
}

//------------------------------------------------
float Hysteresis() {
  return preferences.getFloat("Hysteresis", 2.0);
}

//------------------------------------------------
float AirTempOffset() {
  return preferences.getFloat("AirOffset", 0.0);
}

//------------------------------------------------
float WaterTempOffset() {
  return preferences.getFloat("WaterOffset", 0.0);
}

//------------------------------------------------
// RESET TEMP OSSET IN EEPROM MEMORY
//------------------------------------------------
void ResetOffsetPreferences() {
  printlnD(F("RAZ offset de mesure de température air et eau"));
  // Store the counter to the Preferences
  preferences.putFloat("WaterOffset", 0.0);
  preferences.putFloat("AirOffset", 0.0);
  DumpPreferences();
}

//------------------------------------------------
void SetSummerHour(bool SummerHour) {
  preferences.putBool("Heure ete", SummerHour);
}

//------------------------------------------------
bool SummerHour() {
  return preferences.getBool("Heure ete", 1);
}

//------------------------------------------------
int OffsetHour() {
  return (preferences.getBool("Heure ete", 1)? 0: -1);
}

//------------------------------------------------
// RESET ALL preferences
//------------------------------------------------
void ResetPreferences() {
  printlnD("RAZ des préférences");
  preferences.putBool("Heure ete",      0);
  // SPECIFIQUE
  preferences.putInt("NbPeriodCold",   15);
  preferences.putFloat("Seuil",       1.0);
  preferences.putFloat("Hysteresis",  2.0);
  preferences.putFloat("WaterOffset", 0.0);
  preferences.putFloat("AirOffset",   0.0);
}

//------------------------------------------------
// GET ALL preferences
//------------------------------------------------
//AppPreferences_T GetPreferences() {
//  AppPreferences_T AppPreferences;
//  printlnD("Lecture des préférences");
//  AppPreferences.NbPeriodCold = preferences.getFloat("NbPeriodCold", 15);
//  AppPreferences.Seuil =        preferences.getFloat("Seuil",       1.0);
//  AppPreferences.Hysteresis =   preferences.getFloat("Hysteresis",  2.0);
//  AppPreferences.OffsetAir =    preferences.getFloat("WaterOffset", 0.0);
//  AppPreferences.OffsetEau =    preferences.getFloat("AirOffset",   0.0);
//  return AppPreferences;
//}

//------------------------------------------------
// DUMP ALL preferences
//------------------------------------------------
void DumpPreferences() {
  printlnA();
  printlnA("Préférences");
  printA("  - Heure d'été              = "); printlnA(SummerHour());
  // SPECIFIQUE
  printA("  - Number of period of cold = "); printlnA(NbPeriodCold());
  printA("  - Seuil de déclenchement   = "); printA(Seuil()); printlnA(" °C");
  printA("  - Hysteresis de déclench.  = "); printA(Hysteresis()); printlnA(" °C");
  printA("  - Offset température Eau   = "); printA(WaterTempOffset()); printlnA(" °C");
  printA("  - Offset température Air   = "); printA(AirTempOffset()); printlnA(" °C");
  printlnA();
}
