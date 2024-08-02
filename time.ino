// https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm

// Change Startup Delay for NTP
uint32_t sntp_startup_delay_MS_rfc_not_less_than_60000 () {
  return 60000UL; // 60s
}

// Change NTP Polling Interval
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 () {
  return 12 * 60 * 60 * 1000UL; // 12 hours
}

