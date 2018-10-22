const int eeAddress = 10;
int eeAddressResetDone   = eeAddress;
int eeAddressMaxIntTemp  = eeAddressResetDone   + sizeof(byte);
int eeAddressWaterOffset = eeAddressMaxIntTemp  + sizeof(float);
int eeAddressAirOffset   = eeAddressWaterOffset + sizeof(float);

//------------------------------------------------
// MANAGE WATER TEMP OSSET IN EEPROM MEMORY
//------------------------------------------------
void SetWaterTempOffset(float Offset) {
  float EEPROM_TempOffset = 0.0;
  Serial.print(F("Offset Température Eau = ")); Serial.println(Offset);

  // retreive the EEPROM stored offset
  EEPROM.get(eeAddressWaterOffset, EEPROM_TempOffset);

  // increment
  EEPROM_TempOffset = EEPROM_TempOffset + Offset;

  // store the offset in the EEPROM
  EEPROM.put(eeAddressWaterOffset, EEPROM_TempOffset);
  DumpEEPROM();
}

//------------------------------------------------
float GetWaterTempOffset() {
  float EEPROM_TempOffset = 0.0;

  // retreive the EEPROM stored offset
  EEPROM.get(eeAddressWaterOffset, EEPROM_TempOffset);

  return EEPROM_TempOffset;
}

//------------------------------------------------
// MANAGE AIR TEMP OSSET IN EEPROM MEMORY
//------------------------------------------------
void SetAirTempOffset(float Offset) {
  float EEPROM_TempOffset = 0.0;
  Serial.print(F("Offset Température Air = ")); Serial.println(Offset);

  // retreive the EEPROM stored offset
  EEPROM.get(eeAddressAirOffset, EEPROM_TempOffset);

  // increment
  EEPROM_TempOffset = EEPROM_TempOffset + Offset;

  // store the offset in the EEPROM
  EEPROM.put(eeAddressAirOffset, EEPROM_TempOffset);
  DumpEEPROM();
}

//------------------------------------------------
float GetAirTempOffset() {
  float EEPROM_TempOffset = 0.0;

  // retreive the EEPROM stored offset
  EEPROM.get(eeAddressAirOffset, EEPROM_TempOffset);

  return EEPROM_TempOffset;
}

//------------------------------------------------
// RESET MAX INTERNAL TEMP IN EEPROM MEMORY
//------------------------------------------------
void ResetMaxIntTempEEPROM()
{
  Serial.println(F("RAZ température intérieure max en EEPROM"));
  EEPROM.put(eeAddressMaxIntTemp, 0.0); // uncomment this line to reset EEPROM temperature at next start
  DumpEEPROM();
}

//------------------------------------------------
void SetMaxIntTemp(float IntTemp) {
  float EEPROM_IntTemp = 0.0;

  // retreive the EEPROM stored temperature
  EEPROM.get(eeAddressMaxIntTemp, EEPROM_IntTemp);

  //  Serial.print(F("Temp max Interieur = "));
  //  Serial.print(EEPROM_IntTempR);
  //  Serial.println(" °C");

  // write measured internal temperature to EEPROM
  if (PoolState.IntTemp > EEPROM_IntTemp + 1) {
    // save the last measured temperature
    EEPROM.put(eeAddressMaxIntTemp, PoolState.IntTemp);
  }

  if (PoolState.IntTemp > PoolState.MaxIntTemp) {
    PoolState.MaxIntTemp = PoolState.IntTemp;
  }
}

//------------------------------------------------
float GetMaxIntTemp() {
  float EEPROM_IntTemp = 0.0;

  // retreive the EEPROM stored temperature
  EEPROM.get(eeAddressMaxIntTemp, EEPROM_IntTemp);

  return EEPROM_IntTemp;
}

//------------------------------------------------
// RESET TEMP OSSET IN EEPROM MEMORY
//------------------------------------------------
void ResetOffsetEEPROM() {
  Serial.println(F("RAZ offset de mesure de température air et eau en EEPROM"));
  EEPROM.put(eeAddressWaterOffset, 0.0); // uncomment this line to reset EEPROM temperature at next start
  EEPROM.put(eeAddressAirOffset, 0.0); // uncomment this line to reset EEPROM temperature at next start
  DumpEEPROM();
}

//------------------------------------------------
// RESET ALL EEPROM MEMORY
//------------------------------------------------
void ResetEEPROM() {
  Serial.println("Initialisation à zéro de l'EEPROM");
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  // mark the EEPROM reset flag
  byte Flag = 255;
  EEPROM.put(eeAddressResetDone, Flag);
}

//------------------------------------------------
// dump ALL EEPROM MEMORY
//------------------------------------------------
void DumpEEPROM() {
  Serial.println();
  Serial.println("Dump EEPROM");
  Serial.print(F("  - EEPROM size = ")); Serial.print(EEPROM.length()); Serial.println(" bytes");
  Serial.print(F("  - Etat de EEPROM (reseted) = ")); Serial.println(EEPROMisReseted());
  Serial.print(F("  - Max internal temperature = ")); Serial.print(GetMaxIntTemp()); Serial.println(" °C");
  Serial.print(F("  - Offset température Eau   = ")); Serial.print(GetWaterTempOffset()); Serial.println(" °C");
  Serial.print(F("  - Offset température Air   = ")); Serial.print(GetAirTempOffset()); Serial.println(" °C");
  Serial.println();
}

//------------------------------------------------
// CHECK INITIALIZATION OF EEPROM
//------------------------------------------------
byte EEPROMisReseted() {
  byte Flag;

  // retreive the EEPROM reset flag
  EEPROM.get(eeAddressResetDone, Flag);

  return Flag;
}
