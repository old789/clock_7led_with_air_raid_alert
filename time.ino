// https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm
// https://werner.rothschopf.net/microcontroller/202112_arduino_esp_ntp_rtc_en.htm

// Change Startup Delay for NTP
uint32_t sntp_startup_delay_MS_rfc_not_less_than_60000 () {
  return STARUP_DELAY_FOR_NTP * 60 * 1000UL; 
}

// Change NTP Polling Interval
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 () {
  return poll_interval * 60 * 1000UL;
}

void set_rtc() {
  rtc.refresh();
  time(&now);
  localtime_r(&now, &tm);
  if (  abs( tm.tm_sec - rtc.second() ) > 9 or
        tm.tm_min != rtc.minute() or
        tm.tm_hour != rtc.hour() or
        ( tm.tm_wday + 1 ) != rtc.dayOfWeek() or
        tm.tm_mday != rtc.day() or
        ( tm.tm_mon + 1 ) != rtc.month() or
        (tm.tm_year - 100) != rtc.year() ) {
    rtc.set( tm.tm_sec, tm.tm_min, tm.tm_hour, (tm.tm_wday + 1), tm.tm_mday, (tm.tm_mon + 1), (tm.tm_year - 100) );
#ifdef DEBUG_SERIAL
    Serial.print(F("RTC time was set: "));
    Serial.print( (tm.tm_wday + 1) );
    Serial.print(", ");
    Serial.print(tm.tm_mday);
    Serial.print("/");
    Serial.print( (tm.tm_mon + 1) );
    Serial.print("/");
    Serial.print( (tm.tm_year - 100) );
    Serial.print(" ");
    Serial.print(tm.tm_hour);
    Serial.print(":");
    Serial.print(tm.tm_min);
    Serial.print(":");
    Serial.println(tm.tm_sec);
#endif
  }
  is_rtc_valid = true;
}

void time_is_set() {
#ifdef DEBUG_SERIAL
  Serial.println(F("NTP time was sent!"));
#endif
  is_sntp_valid = true;
  set_rtc();
}

