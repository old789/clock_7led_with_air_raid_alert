void wifi_init(){
#ifdef DBG_WIFI
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passw);             // Connect to the network

  uint16_t i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    i++;
#ifdef DBG_WIFI
    Serial.print(i); Serial.print(' ');
#endif
    if ( i > 300 ) {  // if don't connect then restart
      display.clear();
      display.setSegments(noc,3,1);
      delay(3000);
      ESP.restart();
    }
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

#ifdef DBG_WIFI
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address: ");Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");Serial.println(WiFi.RSSI());
#endif
}
