void check_air_raid_api(){
  uint8_t rc = 0;
  if ( digitalRead(SWITCH_NO_ALARM_MODE) == LOW ) {
    is_air_raid_api_ok = true;
    is_alert_now = false;
    return;
  }
  
  is_air_raid_api_ok = false;
  alert_state = false;

  if ( WiFi.status() != WL_CONNECTED ) {
#ifdef DEBUG_SERIAL
    Serial.println(F("[HTTP] WiFi not connected"));
#endif
    return;
  }

  if ( aerialalerts_api > 0 ) {
    rc = api_aiu();
  } else {
    rc = api_ubi();
  }

  if ( rc > 0 ) {
    return;
  }

  if ( alert_state ) { 
    if ( ! is_alert_now ) {
      is_alert_now = true;
#ifdef DEBUG_SERIAL
      Serial.println(F("Alert!"));
#endif
    }
  } else {
    if ( is_alert_now ) {
      is_alert_now = false;
#ifdef DEBUG_SERIAL
      Serial.println(F("Alert cancelled"));
#endif
    }
  }
  is_air_raid_api_ok = true;

}

uint8_t api_aiu(){
  char s[33] = {0};
  WiFiClient client;
  HTTPClient http;

#ifdef DEBUG_HTTP
  Serial.printf("[HTTP] begin...\r\n[HTTP] uri = %s\r\n", aiu_uri);
#endif
  
  if ( ! http.begin(client, aiu_uri)) {
#ifdef DEBUG_SERIAL
    Serial.println(F("[HTTP] Unable to connect"));
#endif
    return(1);
  }

#ifdef DEBUG_HTTP
  Serial.println(F("[HTTP] send GET request..."));
#endif
  int httpCode = http.GET();
#ifdef DEBUG_HTTP
  Serial.printf("[HTTP] GET done with code: %d\r\n", httpCode);
#endif

  if (httpCode < 0) {
#ifdef DEBUG_SERIAL
    Serial.printf("[HTTP] GET failed, error: %s\r\n", http.errorToString(httpCode).c_str());
#endif
    http.end();
    return(1);
  }
  
  if ( ! ( httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY ) ) {
    http.end();
    return(1);
  }
  
  String payload = http.getString();
  http.end();
#ifdef DEBUG_HTTP
  Serial.printf("[HTTP] Got %dB payload \"%s\"\r\n", payload.length(), payload);
#endif
  if ( payload.length() > 1 ){
#ifdef DEBUG_HTTP
    Serial.println("[HTTP] Payload to long");
#endif
    return(2);
  }

  payload.toCharArray(s, sizeof(s));
  if ( s[0] == 'N' ) {
    alert_state = false;
  }else if ( s[0] == 'A' ) {
    alert_state = true;
  }else{
#ifdef DEBUG_HTTP
    Serial.println("[HTTP] Unknown alert state");
#endif
    return(2);
  }
  return(0);
}

uint8_t api_ubi(){
  WiFiClient client;
  HTTPClient http;
  JsonDocument jroot;
  JsonDocument jfilter;

#ifdef DEBUG_HTTP
  Serial.println(F("[HTTP] begin..."));
#endif

  if ( ! http.begin(client, AIR_RAID_API_URL_UBI)) {
#ifdef DEBUG_SERIAL
    Serial.println(F("[HTTP] Unable to connect"));
#endif
    return(1);
  }

#ifdef DEBUG_HTTP
  Serial.println(F("[HTTP] send GET request..."));
#endif
  int httpCode = http.GET();
#ifdef DEBUG_HTTP
  Serial.printf("[HTTP] GET done with code: %d\r\n", httpCode);
#endif

  if (httpCode < 0) {
#ifdef DEBUG_SERIAL
    Serial.printf("[HTTP] GET failed, error: %s\r\n", http.errorToString(httpCode).c_str());
#endif
    http.end();
    return(1);
  }
  
  if ( ! ( httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY ) ) {
    http.end();
    return(1);
  }
  
  String payload = http.getString();
#ifdef DEBUG_HTTP
  Serial.printf("[HTTP] Got %dB payload\r\n",payload.length());
#endif

  jfilter["states"][region_name[region]]["alertnow"] = true;
  DeserializationError jerror = deserializeJson(jroot, payload, DeserializationOption::Filter(jfilter));
  if ( jerror ) {
#ifdef DEBUG_SERIAL
      Serial.printf("[HTTP] deserializeJson() failed: %s\r\n", jerror.f_str());
#endif
    http.end();
    return(2);
#ifdef DEBUG_HTTP
  } else {
    Serial.println(F("[HTTP] JSON deserialized successfully"));
#endif
  }

  alert_state = jroot["states"][region_name[region]]["alertnow"];
  http.end();
  return(0);
}

