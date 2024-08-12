void wifi_init(){
#ifdef DBG_WIFI
  Serial.print(F("Connecting to "));
  Serial.print(ssid);
  Serial.println(F(" ..."));
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passw);             // Connect to the network

  uint8_t w = 0;
  uint16_t i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    i++;
#ifdef DBG_WIFI
    Serial.print(i); Serial.print(' ');
#endif
    display.setSegments(wave[w^=1]);
    if ( i > 300 ) {
      display.clear();
      display.setSegments(noc,3,1);
#ifdef DBG_WIFI
      Serial.println('\n');
      Serial.println(F("Connection d'not established!"));
#endif
      delay(3000);
      // ESP.restart();
      return;
    }
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

#ifdef DBG_WIFI
  Serial.println('\n');
  Serial.println(F("Connection established!"));
  Serial.print(F("IP address: "));Serial.println(WiFi.localIP());
  Serial.print(F("RSSI: "));Serial.println(WiFi.RSSI());
#endif
}
