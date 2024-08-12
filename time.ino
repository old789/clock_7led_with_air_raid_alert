// https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm
// https://werner.rothschopf.net/microcontroller/202112_arduino_esp_ntp_rtc_en.htm

// Change Startup Delay for NTP
uint32_t sntp_startup_delay_MS_rfc_not_less_than_60000 () {
  return 60000UL; // 60s
}

// Change NTP Polling Interval
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 () {
  return poll_interval * 60 * 1000UL;
}

void time_is_set() {
#ifdef DEBUG_SERIAL
  Serial.println(F("NTP time was sent!"));
#endif
  is_sntp_valid = true;
  //set_rtc();
}

void set_rtc() {
  rtc.refresh();
  rtc.set( tm.tm_sec, tm.tm_min, tm.tm_hour, tm.tm_wday, tm.tm_mday, (tm.tm_mon + 1), (tm.tm_year + 1900) );
}
