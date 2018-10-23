//--------------------------------------------------------------------
// INITIALISATION DES CAPTEURS DE TEMPERATURE
//--------------------------------------------------------------------
void InitTemperatureSensors() {
  Serial.println(F("Dallas Temperature IC Control Library initialization"));
  // Start up the library
  DallasSensors.begin();

  // locate devices on the 1-Wire bus
  Serial.print(F("Locating 1-Wire devices..."));
  Serial.print("Found ");
  Serial.print(DallasSensors.getDeviceCount(), DEC);
  Serial.print(" devices.");
  Serial.print(" Found ");
  DallasDeviceCount = DallasSensors.getDS18Count();
  Serial.print(DallasDeviceCount, DEC);
  Serial.println(" Dallas devices.");
  ErrorTemp = (DallasDeviceCount != 2);

  // clear the display
  display.clear();
  Ypos = 0;
  DisplayOneMoreLine("Found " + String(DallasDeviceCount) + " Dallas devices", TEXT_ALIGN_LEFT);

  if (DallasSensors.getAddress(Device0_Thermometer, 0)) {
    Serial.print ("Device 0 address: ");
    print1wireAddress(Device0_Thermometer);
    Serial.println ();
    Display1wireAddress("Device 0 address: ", Device0_Thermometer);
    delay (2000);
  }
  else {
    Serial.println(F("Unable to find address for Device 0"));
    delay (2000);
    DisplayAlert("Unable to find address for Device 0");
    ErrorTemp0 = true;
    ErrorTemp = true;
  }
  if (DallasSensors.getAddress(Device1_Thermometer, 1)) {
    Serial.print ("Device 1 address : ");
    print1wireAddress(Device1_Thermometer);
    Serial.println ();
    Display1wireAddress("Device 1 address : ", Device1_Thermometer);
    delay (2000);
  }
  else {
    Serial.println(F("Unable to find address for Device 1"));
    delay (2000);
    DisplayAlert("Unable to find address for Device 1");
    ErrorTemp1 = true;
    ErrorTemp = true;
  }

  // report parasite power requirements
  Serial.print("Parasite power is : ");
  if (DallasSensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // set the resolution to N bit per device
  //  DallasSensors.setResolution(Device0_Thermometer, TEMPERATURE_PRECISION);
  //  DallasSensors.setResolution(Device1_Thermometer, TEMPERATURE_PRECISION);
  DallasSensors.setResolution(TEMPERATURE_PRECISION);

  if (!ErrorTemp0) {
    Serial.print("Device 0 Resolution : ");
    Serial.println(DallasSensors.getResolution(Device0_Thermometer), DEC);
  }
  if (!ErrorTemp1) {
    Serial.print("Device 1 Resolution : ");
    Serial.println(DallasSensors.getResolution(Device1_Thermometer), DEC);
  }

  // Get the initial temperatures
  DallasSensors.requestTemperatures(); // Send the command to get temperature readings
  PoolState.AirTemp = DallasSensors.getTempCByIndex(ONE_WIRE_AIR_TEMP_DEVICE) + AirTempOffset();
  Serial.print("Air Temperature is : "); Serial.println(PoolState.AirTemp);
  DisplayOneMoreLine("Temp Air : " + String(PoolState.AirTemp) + " °C", TEXT_ALIGN_LEFT);
  PoolState.WaterTemp = DallasSensors.getTempCByIndex(ONE_WIRE_WATER_TEMP_DEVICE) + WaterTempOffset();
  Serial.print("Eau Temperature is : "); Serial.println(PoolState.WaterTemp);
  DisplayOneMoreLine("Temp Eau : " + String(PoolState.WaterTemp) + " °C", TEXT_ALIGN_LEFT);

  // Timer sampling temperatures
  TimerTemp = timer.setInterval(intervalTemp, SampleTemperatures);
  Serial.print(F("Initialisation timer échantillonage températures = "));
  Serial.println(millis());
  DisplayOneMoreLine("Init timer temp sampling", TEXT_ALIGN_LEFT);
  delay (2000); // décaler les timers USB, Temp et Wi-Fi
  timer.restartTimer(TimerTemp);
}

//**********************************************
//  function to print a 1-Wire device address
//**********************************************
void print1wireAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

//*******************************************************
//  function to convert a 1-Wire device address to String
//*******************************************************
String String1wireAddress(DeviceAddress deviceAddress)
{
  String address = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) address += "0";
    address += String(deviceAddress[i], HEX);
  }
  return address;
}

//***************************************************
// function to print the temperature for a device
//***************************************************
void print1wireTemperature(DeviceAddress deviceAddress)
{
  float tempC = DallasSensors.getTempC(deviceAddress);
  Serial.print("Temp C : ");
  Serial.println(tempC);
}

//*****************************************
//  display water temperature with RGB LED
//*****************************************
void DisplayWaterTemperatureOnLED (int WaterTemp)
{
  int const Tmin = 20 * 10;
  int const Tmax = 25 * 10;
  int Wtemp = WaterTemp * 10;

  // map the temperature to the range of the analog out:
  Wtemp = constrain(Wtemp, Tmin, Tmax);
  TempLEDred = map(Wtemp, Tmin, Tmax, 255, 0);
  TempLEDgreen = 255 - TempLEDred;
  TempLEDblue = 0;

  // change the LED color between red (cold) and green (hot):
  //  analogWrite(pTempLEDred,   TempLEDred);
  //  analogWrite(pTempLEDgreen, TempLEDgreen);
  //  analogWrite(pTempLEDblue,  TempLEDblue);
  ledcWrite(cTempLEDred,   TempLEDred);
  ledcWrite(cTempLEDgreen, TempLEDgreen);
  ledcWrite(cTempLEDblue,  TempLEDblue);
}

//**********************************************
// Acquire temperature from thermistor DALLAS
//**********************************************
void AcquireTemperatures()
{
  // read the value from the sensor: Air Temperature
  //AirTemp = TemperatureFrom3950NTC (pAirR) + TEMPOFFSETAIR;
  Temp = DallasSensors.getTempCByIndex(ONE_WIRE_AIR_TEMP_DEVICE) + AirTempOffset();
  if (abs(Temp - PoolState.AirTemp) < 10)
    PoolState.AirTemp = Temp;
  else {
#if defined USB_OUTPUT
    Serial.print("Gros écart de température Air: Air Temp = ");
    Serial.println(Temp);
#endif
  }
  //#if defined DEBUG
  //Serial.print("Air Temperature is: ");
  //Serial.println(AirTemp);
  //#endif

  // read the value from the sensor: Water Temperature
  //WaterTemp = TemperatureFrom3950NTC (pWaterR) + TEMPOFFSETWATER;
  Temp = DallasSensors.getTempCByIndex(ONE_WIRE_WATER_TEMP_DEVICE) + WaterTempOffset();
  if (abs(Temp - PoolState.WaterTemp) < 10)
    PoolState.WaterTemp = Temp;
  else {
#if defined USB_OUTPUT
    Serial.print("Gros écart de température Eau: Eau Temp = ");
    Serial.println(Temp);
#endif
  }
  //#if defined DEBUG
  //Serial.print("Water Temperature is: ");
  //Serial.println(WaterTemp);
  //#endif

}

//**********************************************
// Sample temperature from thermistor DALLAS
//**********************************************
void SampleTemperatures()
{
  if (!ErrorTemp) {
    // Send the command to get temperature readings
    DallasSensors.setWaitForConversion(false);  // makes it async
    DallasSensors.requestTemperatures();
    DallasSensors.setWaitForConversion(true);

    timer.setTimeout(delaySamplingTemp, AcquireTemperatures);
    //Serial.println("lancement acquisition temp");
  }
}

//**********************************************
//  mesure temperature from thermistor TMP36
//**********************************************
//float TemperatureFromTMP36(int ThermistorPin)
//{
//  //getting the voltage reading from the temperature sensor
//  int reading = analogRead(ThermistorPin);
//  //Serial.print("TMP36 reading = "); Serial.println(reading);
//  // converting that reading to voltage, for 3.3v arduino use 3.3
//  float voltage = reading * AREF_VOLTAGE;
//  voltage /= 1024.0;
//  // now the temperature
//  float temperatureC = (voltage - 0.5) * 100 ;
//  //converting from 10 mv per degree with 500 mV offset
//  //to degrees ((voltage - 500mV) times 100)
//  return temperatureC;
//}
