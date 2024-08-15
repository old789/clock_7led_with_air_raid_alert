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

bool compare_date_part(unsigned int t1, unsigned int t2, uint8_t ds){
#ifdef DEBUG_SERIAL
  char stmp[128] = { 0 };
  const char* descr[4] = { "week day", "day", "month", "year" };
  sprintf(stmp, "compare %ss: sys %u rtc %u", descr[ds], t1, t2);
  Serial.println(stmp);
#endif
  if ( t1 != t2 ) {
#ifdef DEBUG_SERIAL
    memset(stmp, 0, sizeof(stmp));
    sprintf(stmp, "%ss are different, a RTC needs the correction", descr[ds]);
    Serial.println(stmp);
#endif
    return(false);
  }
  return(true);
}

void set_rtc() {
  bool no_need_rtc_set = true;
  unsigned int t_sys;
  unsigned int t_rtc;
  int t_dif;
  
#ifdef DEBUG_SERIAL
  char stmp[128] = { 0 };
#endif
  rtc.refresh();
  time(&now);
  localtime_r(&now, &tm);

  t_sys = tm.tm_sec + 60 * tm.tm_min + 3600 * tm.tm_hour;
  t_rtc = rtc.second() + 60 * rtc.minute() + 3600 * rtc.hour();
  t_dif = t_sys - t_rtc;

#ifdef DEBUG_SERIAL
  // memset(stmp, 0, sizeof(stmp));
  sprintf(stmp, "compare time: sys %u:%u:%u (%u) rtc %u:%u:%u (%u)", 
    tm.tm_hour, tm.tm_min, tm.tm_sec, t_sys, rtc.hour(),rtc.minute(), rtc.second(), t_rtc);
  Serial.println(stmp);
#endif
  if ( abs( t_dif ) > 9 ) {
    no_need_rtc_set = false;
#ifdef DEBUG_SERIAL
    memset(stmp, 0, sizeof(stmp));
    sprintf(stmp, "time difference is %ds, rtc needs the correction", t_dif);
    Serial.println(stmp);
#endif
  }

  no_need_rtc_set = no_need_rtc_set and
    compare_date_part(( tm.tm_wday + 1 ), rtc.dayOfWeek(), 0 ) and
    compare_date_part( tm.tm_mday, rtc.day(), 1 ) and
    compare_date_part(( tm.tm_mon + 1 ), rtc.month(), 2 ) and
    compare_date_part(( tm.tm_year - 100 ), rtc.year(), 3 );
  
  if ( ! no_need_rtc_set ) {
    rtc.set( tm.tm_sec, tm.tm_min, tm.tm_hour, (tm.tm_wday + 1), tm.tm_mday, (tm.tm_mon + 1), (tm.tm_year - 100) );
#ifdef DEBUG_SERIAL
    memset(stmp, 0, sizeof(stmp));
    sprintf(stmp, "a RTC time was set: %u, %u/%u/%u %u:%u:%u", 
      (tm.tm_wday + 1), tm.tm_mday, (tm.tm_mon + 1), (tm.tm_year - 100), tm.tm_hour, tm.tm_min, tm.tm_sec);
    Serial.println(stmp);
  } else {
    Serial.println(F("everything is Ok, a RTC no need the correction"));
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
#ifdef DEBUG_SERIAL
  } else {
      Serial.println(F("Checking for a valid NTP time was discarded - time is already invalid"));
#endif
  }

}
