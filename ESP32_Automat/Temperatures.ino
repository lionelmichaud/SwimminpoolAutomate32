//--------------------------------------------------------------------
// INITIALISATION DES CAPTEURS DE TEMPERATURE
//--------------------------------------------------------------------
void InitTemperatureSensors(Configuration_T Config) {
  delay (2000); // retarder l'init des thermistance
  printlnA(F("Initializing Dallas Temperature IC Control Library..."));
  // Start up the library
  DallasSensors.begin();

  delay (2000); // retarder l'init des thermistance
  // locate devices on the 1-Wire bus
  printA(F("Locating 1-Wire devices..."));
  printA("Found ");
  printA(DallasSensors.getDeviceCount());
  printA(" devices.");
  printA(" Found ");
  DallasDeviceCount = DallasSensors.getDS18Count();
  printA(DallasDeviceCount);
  printlnA(" Dallas devices.");
  //  PoolState.ErrorTempSensorInit = (DallasDeviceCount != 2);

  // clear the display
  display.clear();
  Ypos = 0;
  DisplayOneMoreLine("Found " + String(DallasDeviceCount) + " Dallas devices", TEXT_ALIGN_LEFT);

  if (DallasSensors.getAddress(Device0_Thermometer, 0)) {
    printA ("Device 0 address: ");
    print1wireAddress(Device0_Thermometer);
    printlnA ();
    Display1wireAddress("Device 0: ", Device0_Thermometer);
    delay (2000);
  }
  else {
    printlnA(F("  Unable to find address for Device 0"));
    delay (2000);
    DisplayAlert("Unable to find address for Device 0");
    PoolState.ErrorTempSensorInit0 = true;
    PoolState.ErrorTempSensorInit = true;
  }
  if (DallasSensors.getAddress(Device1_Thermometer, 1)) {
    printA ("Device 1 address : ");
    print1wireAddress(Device1_Thermometer);
    printlnA ();
    Display1wireAddress("Device 1: ", Device1_Thermometer);
    delay (2000);
  }
  else {
    printlnA(F("  Unable to find address for Device 1"));
    delay (2000);
    DisplayAlert("Unable to find address for Device 1");
    PoolState.ErrorTempSensorInit1 = true;
    PoolState.ErrorTempSensorInit = true;
  }

  if (DallasSensors.getAddress(Device2_Thermometer, 2)) {
    printA ("Device 2 address : ");
    print1wireAddress(Device2_Thermometer);
    printlnA ();
    Display1wireAddress("Device 2: ", Device2_Thermometer);
    delay (2000);
  }
  else {
    printlnA(F("  Unable to find address for Device 2"));
    delay (2000);
    DisplayAlert("Unable to find address for Device 2");
    PoolState.ErrorTempSensorInit2 = true;
    //PoolState.ErrorTempSensorInit = true;
  }

  // report parasite power requirements
  printA("Parasite power is : ");
  if (DallasSensors.isParasitePowerMode()) {
    printlnA("ON");
  } else {
   printlnA("OFF");
  }

  // set the resolution to N bit per device
  DallasSensors.setResolution(Device0_Thermometer, TEMPERATURE_PRECISION);
  DallasSensors.setResolution(Device1_Thermometer, TEMPERATURE_PRECISION);
  DallasSensors.setResolution(Device2_Thermometer, TEMPERATURE_PRECISION);
  //DallasSensors.setResolution(TEMPERATURE_PRECISION);

  delay (2000); // retarder l'init des thermistance
  if (!PoolState.ErrorTempSensorInit0) {
    printA("Device 0 Resolution : ");
    printlnA(DallasSensors.getResolution(Device0_Thermometer));
  }
  if (!PoolState.ErrorTempSensorInit1) {
    printA("Device 1 Resolution : ");
    printlnA(DallasSensors.getResolution(Device1_Thermometer));
  }
  if (!PoolState.ErrorTempSensorInit2) {
    printA("Device 2 Resolution : ");
    printlnA(DallasSensors.getResolution(Device2_Thermometer));
  }

  // Get the initial temperatures
  DallasSensors.requestTemperatures(); // Send the command to get temperature readings
  if (!PoolState.ErrorTempSensorInit) {
    // Température de l'air
    PoolState.AirTemp = DallasSensors.getTempCByIndex(ONE_WIRE_AIR_TEMP_DEVICE) + AirTempOffset();
    printA("Air Temperature is : "); printlnA(PoolState.AirTemp);
    DisplayOneMoreLine("Temp Air : " + String(PoolState.AirTemp) + " °C", TEXT_ALIGN_LEFT);
    // Température de l'eau
    PoolState.WaterTemp = DallasSensors.getTempCByIndex(ONE_WIRE_WATER_TEMP_DEVICE) + WaterTempOffset();
    printA("Eau Temperature is : "); printlnA(PoolState.WaterTemp);
    DisplayOneMoreLine("Temp Eau : " + String(PoolState.WaterTemp) + " °C", TEXT_ALIGN_LEFT);
    // Température intérieure
    PoolState.InternalTemp = DallasSensors.getTempCByIndex(ONE_WIRE_INTERNAL_TEMP_DEVICE);
    printA("Internal Temperature is : "); printlnA(PoolState.InternalTemp);
    //DisplayOneMoreLine("Temp Eau : " + String(PoolState.InternalTemp) + " °C", TEXT_ALIGN_LEFT);
  }
  // Timer sampling temperatures
  TimerTemp = timer.setInterval(Config.intervalTemp, SampleTemperatures);
  printA("Initialisation timer échantillonage températures = ");
  printlnA(millis());
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
    if (deviceAddress[i] < 16) printA("0");
    printA(deviceAddress[i], HEX);
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
  printA("Temp C : ");
  printlnA(tempC);
}

//*****************************************
//  display water temperature with RGB LED
//*****************************************
void DisplayWaterTemperatureOnLED (int WaterTemp, Configuration_T Config)
{
  int Tmin  = Config.RedLEDtemp   * 10;
  int Tmax  = Config.GreenLEDtemp * 10;
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
  if (abs(Temp - PoolState.AirTemp) < 10) {
    PoolState.AirTemp = Temp;
    PoolState.ErrorTempAir = false;
  }
  else {
    PoolState.ErrorTempAir = true;
    printW("Gros écart de température Air: Air Temp = ");
    printlnW(Temp);
  }
  //  printD("Air Temperature is: ");
  //  printlnD(PoolState.AirTemp);

  // read the value from the sensor: Water Temperature
  //WaterTemp = TemperatureFrom3950NTC (pWaterR) + TEMPOFFSETWATER;
  Temp = DallasSensors.getTempCByIndex(ONE_WIRE_WATER_TEMP_DEVICE) + WaterTempOffset();
  if (abs(Temp - PoolState.WaterTemp) < 10) {
    PoolState.WaterTemp = Temp;
    PoolState.ErrorTempWater = false;
  }
  else {
    PoolState.ErrorTempWater = true;
    printW("Gros écart de température Eau: Eau Temp = ");
    printlnW(Temp);
  }
  // température intérieure
  PoolState.InternalTemp = DallasSensors.getTempCByIndex(ONE_WIRE_INTERNAL_TEMP_DEVICE);
}

//**********************************************
// Sample temperature from thermistor DALLAS
//**********************************************
void SampleTemperatures()
{
  if (!PoolState.ErrorTempSensorInit) {
    // Send the command to get temperature readings
    DallasSensors.setWaitForConversion(false);  // makes it async
    DallasSensors.requestTemperatures();
    DallasSensors.setWaitForConversion(true);

    timer.setTimeout(delaySamplingTemp, AcquireTemperatures);
    //printlnD("lancement acquisition temp");
  }
}

//**********************************************
//  mesure temperature from thermistor TMP36
//**********************************************
//float TemperatureFromTMP36(int ThermistorPin)
//{
//  //getting the voltage reading from the temperature sensor
//  int reading = analogRead(ThermistorPin);
//  //printD("TMP36 reading = "); printlnD(reading);
//  // converting that reading to voltage, for 3.3v arduino use 3.3
//  float voltage = reading * AREF_VOLTAGE;
//  voltage /= 1024.0;
//  // now the temperature
//  float temperatureC = (voltage - 0.5) * 100 ;
//  //converting from 10 mv per degree with 500 mV offset
//  //to degrees ((voltage - 500mV) times 100)
//  return temperatureC;
//}
