//--------------------------------------------------------------------
void sendDomoticz(String url) {
  printlnV(">Send data to Domoticz:");
  printV(">  connecting to ");
  printlnV(Configuration.domoticz.host);
  printV(">  Requesting URL: ");
  printlnV(url);
//  http.begin(Configuration.domoticz.host, Configuration.domoticz.port, url);
//  int httpCode = http.GET();
//  if (httpCode) {
//    if (httpCode == 200) {
//      String payload = http.getString();
//      printlnD(">  Domoticz response =");
//      printlnD(payload);
//    }
//  }
  printlnV(">  closing connection");
  //  http.end();
}

//--------------------------------------------------------------------
void sendSvalueDomoticz(String svalue, int idx) {
  String url;
  url = "/json.htm?type=command&param=udevice&idx="; //username=MkE=&password=OVM=&
  url += String(idx);
  url += "&nvalue=0&svalue="; url += svalue;
  url += "&rssi=";
  url += String(100 + bestRSSI);
  url += "&battery=";
  url += String(255);
  sendDomoticz(url);
}

//--------------------------------------------------------------------
void sendSwitchCmdDomoticz(String switchcmd, int idx) {
  String url;
  url = "/json.htm?type=command&param=switchlight&idx=";
  url += String(idx);
  url += "&switchcmd="; url += switchcmd;
  url += "&rssi=";
  url += String(100 + bestRSSI);
  url += "&battery=";
  url += String(255);
  sendDomoticz(url);
}
