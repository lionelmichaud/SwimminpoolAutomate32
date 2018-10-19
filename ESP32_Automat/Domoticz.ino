//--------------------------------------------------------------------
void sendDomoticz(String url) {
#if defined DEBUG
  Serial.println(">Send data to Domoticz:");
  Serial.print(">  connecting to ");
  Serial.println(host);
  Serial.print(">  Requesting URL: ");
  Serial.println(url);
#endif
  http.begin(host, port, url);
  int httpCode = http.GET();
  if (httpCode) {
    if (httpCode == 200) {
      String payload = http.getString();
#if defined DEBUG
      Serial.println(">  Domoticz response =");
      Serial.println(payload);
#endif
    }
  }
#if defined DEBUG
  Serial.println(">  closing connection");
#endif
  //  http.end();
}

void sendSvalueDomoticz(String svalue, String idx) {
  String url;
  url = "/json.htm?type=command&param=udevice&idx="; //username=MkE=&password=OVM=&
  url += idx;
  url += "&nvalue=0&svalue="; url += svalue;
  url += "&rssi=";
  url += String(100 + bestRSSI);
  url += "&battery=";
  url += String(255);
  sendDomoticz(url);
}

void sendSwitchCmdDomoticz(String switchcmd, String idx) {
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
