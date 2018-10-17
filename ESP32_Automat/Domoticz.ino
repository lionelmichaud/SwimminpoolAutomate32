//--------------------------------------------------------------------
void sendDomoticz(String url) {
#if defined DEBUG
  Serial.println(">Send data to Domoticz:");
  Serial.print(">  connecting to ");
  Serial.println(host);
  Serial.print(">  Requesting URL: ");
  Serial.println(url);
#endif
//  http.begin(host, port, url);
//  int httpCode = http.GET();
//  if (httpCode) {
//    if (httpCode == 200) {
//      String payload = http.getString();
//#if defined DEBUG
//      Serial.println(">  Domoticz response =");
//      Serial.println(payload);
//#endif
//    }
//  }
#if defined DEBUG
  Serial.println(">  closing connection");
#endif
//  http.end();
}
