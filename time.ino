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
  bool no_need_rtc_set = true;
#ifdef DEBUG_SERIAL
  char stmp[128] = { 0 };
#endif
  rtc.refresh();
  time(&now);
  localtime_r(&now, &tm);

#ifdef DEBUG_SERIAL
  memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "Compare seconds: sys %u rtc %u", tm.tm_sec, rtc.second());
  Serial.println(stmp);
#endif
  if ( abs( tm.tm_sec - rtc.second() ) > 9 ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
  Serial.println(F("Seconds are different, need the correction"));
#endif
  }

#ifdef DEBUG_SERIAL
  memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "Compare minutes: sys %u rtc %u", tm.tm_min, rtc.minute());
  Serial.println(stmp);
#endif
  if ( tm.tm_min != rtc.minute() ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
  Serial.println(F("Minutes are different, need the correction"));
#endif
  }

#ifdef DEBUG_SERIAL
  memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "Compare hours: sys %u rtc %u", tm.tm_hour, rtc.hour());
  Serial.println(stmp);
#endif
  if ( tm.tm_hour != rtc.hour() ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
  Serial.println(F("Hours are different, need the correction"));
#endif
  }

#ifdef DEBUG_SERIAL
  memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "Compare week days: sys %u rtc %u", ( tm.tm_wday + 1 ), rtc.dayOfWeek());
  Serial.println(stmp);
#endif
  if ( ( tm.tm_wday + 1 ) != rtc.dayOfWeek() ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
  Serial.println(F("Week days are different, need the correction"));
#endif
  }

#ifdef DEBUG_SERIAL
  memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "Compare days: sys %u rtc %u", tm.tm_mday, rtc.day());
  Serial.println(stmp);
#endif
  if ( tm.tm_mday != rtc.day() ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
  Serial.println(F("Days are different, need the correction"));
#endif
  }

#ifdef DEBUG_SERIAL
  memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "Compare months: sys %u rtc %u", ( tm.tm_mon + 1 ), rtc.month());
  Serial.println(stmp);
#endif
  if ( ( tm.tm_mon + 1 ) != rtc.month() ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
  Serial.println(F("Months are different, need the correction"));
#endif
  }

#ifdef DEBUG_SERIAL
  memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "Compare years: sys %u rtc %u", ( tm.tm_year - 100 ), rtc.year());
  Serial.println(stmp);
#endif
  if ( ( tm.tm_year - 100 ) != rtc.year() ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
  Serial.println(F("Years are different, need the correction"));
#endif
  }

/*
 * This is better code but without debugging
 *
  if (  abs( tm.tm_sec - rtc.second() ) > 9 or
        tm.tm_min != rtc.minute() or
        tm.tm_hour != rtc.hour() or
        ( tm.tm_wday + 1 ) != rtc.dayOfWeek() or
        tm.tm_mday != rtc.day() or
        ( tm.tm_mon + 1 ) != rtc.month() or
        (tm.tm_year - 100) != rtc.year() ) {
*/
  if ( ! no_need_rtc_set ) {
    rtc.set( tm.tm_sec, tm.tm_min, tm.tm_hour, (tm.tm_wday + 1), tm.tm_mday, (tm.tm_mon + 1), (tm.tm_year - 100) );
#ifdef DEBUG_SERIAL
    memset(stmp, 0, sizeof(stmp));
    sprintf(stmp, "RTC time was set: %u, %u/%u/%u %u:%u:%u", 
      (tm.tm_wday + 1), tm.tm_mday, (tm.tm_mon + 1), (tm.tm_year - 100), tm.tm_hour, tm.tm_min, tm.tm_sec);
    Serial.println(stmp);
  } else {
    Serial.println(F("Everything is Ok, rtc no need the correction"));
#endif
  }
  is_rtc_valid = true;
}

void time_is_set() {
#ifdef DEBUG_SERIAL
  Serial.println(F("NTP time was sent!"));
#endif
  is_sntp_valid = true;
  time(&last_sntp_sync);
  set_rtc();
}

void check_is_sntp_valid(){
  if ( is_sntp_valid ) {
    time(&now);
#ifdef DEBUG_SERIAL
  Serial.print(F("Time now: "));
  Serial.print((uint64_t)now);
  Serial.print(F(" last time got NTP: "));
  Serial.print((uint64_t)last_sntp_sync);
  Serial.print(F(" seconds ago: "));
  Serial.println( (now - last_sntp_sync) );
#endif
    if ( ( now - last_sntp_sync ) > PAUSE_BEFORE_NTP_TIME_WILL_NO_VALID ) {
      is_sntp_valid = false;
#ifdef DEBUG_SERIAL
      Serial.println(F("NTP time is non valid"));
#endif
    }
  }
#ifdef DEBUG_SERIAL
    else {
      Serial.println(F("Checking for a valid NTP time was discarded - time is already invalid"));
    }
#endif

}
