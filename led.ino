void led_alert() {

  if ( is_alert_now ) {
    switch ( big_led_bright_direction ) {
      case 0:
        big_led_brightness_up();
        break;
      case 1:
        big_led_brightness_down();
        break;
      default:  // just because
        big_led_pause();
    }
  } else {  
    analogWrite( LED_ALARM, 0);
  }
}

void big_led_brightness_up() {
  big_led_cur_bright += BIG_LED_BRIGHT_STEP;
  if ( big_led_cur_bright > BIG_LED_MAX_BRIGHT ) {
    big_led_cur_bright = BIG_LED_MAX_BRIGHT;
    big_led_bright_direction^=1;
  }
  analogWrite( LED_ALARM, big_led_cur_bright);
}

void big_led_brightness_down() {
  big_led_cur_bright -= BIG_LED_BRIGHT_STEP;
  if ( big_led_cur_bright < BIG_LED_MIN_BRIGHT ) {
    big_led_cur_bright = BIG_LED_MIN_BRIGHT;
    big_led_bright_direction^=1;
  }
  analogWrite( LED_ALARM, big_led_cur_bright);
}

void big_led_pause(){
  big_led_cur_pause++;
  if ( big_led_cur_pause > TICS_BIG_LED_PAUSE ) {
    big_led_cur_pause = 0;
    big_led_bright_direction++;
    if ( big_led_bright_direction > 3 ) {
      big_led_bright_direction = 0;
    }
  }
}

/*
void led_alert() {
  if ( is_alert_now ) {
    if ( digitalRead(LED_ALARM) == LOW ) {
      digitalWrite( LED_ALARM, HIGH);
#ifdef DEBUG_SERIAL
      Serial.println(F("Big LED On"));
#endif
    }
  } else {
    if ( digitalRead(LED_ALARM) == HIGH ) {
        digitalWrite( LED_ALARM, LOW);
#ifdef DEBUG_SERIAL
        Serial.println(F("Big LED Off"));
#endif
    }
  }
}
*/
