#if defined NTP
void Send_NTP_to_Arduino() {
  Serial.println(">Time = " + TimeNTP);
}
#endif

