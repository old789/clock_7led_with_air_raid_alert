
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
  big_led_cur_bright += choice_big_led_bright_step(big_led_cur_bright);
  if ( big_led_cur_bright >= BIG_LED_MAX_BRIGHT ) {
    // big_led_cur_bright = BIG_LED_MAX_BRIGHT;
    big_led_bright_direction^=1;
  }
  analogWrite( LED_ALARM, big_led_cur_bright);
}

void big_led_brightness_down() {
  big_led_cur_bright -= choice_big_led_bright_step(big_led_cur_bright);
  if ( big_led_cur_bright <= BIG_LED_MIN_BRIGHT ) {
    // big_led_cur_bright = BIG_LED_MIN_BRIGHT;
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

uint8_t choice_big_led_bright_step( int16_t current_bright ){
  if ( current_bright < 7 ) 
    return(1);
  else if ( current_bright < 75 )
    return(10);
  else 
    return(30);
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

/*
void led_alert(){
  char inChar[2] = {0};  // because strncpy/strncat needs \0-terminated 2nd argument
  if (Serial.available()) {
    inChar[0] = Serial.read();
  } else {
    return;
  }
              // Serial.print("Read symbol \"");
              // Serial.print( inChar[0] );
              // Serial.println("\"");
  switch ( inChar[0] ) {
    case 'q':
              big_led_cur_bright += 10;
              break;
    case 'a':
              big_led_cur_bright -= 10;
              break;
    case 'w':
              big_led_cur_bright++;
              break;
    case 's': 
              big_led_cur_bright--;
              break;
    default:
              Serial.print("Unknown symbol \"");
              Serial.print( inChar[0] );
              Serial.println("\"");
              return;
  }
  
  if ( big_led_cur_bright > 0xff ) 
    big_led_cur_bright = 0xff;
  else if ( big_led_cur_bright < 0 )
    big_led_cur_bright = 0;

  analogWrite( LED_ALARM, big_led_cur_bright);

  Serial.print("Current brightness ");
  Serial.print( big_led_cur_bright );
  Serial.println();
}
*/
