
void led_alert() {
  if ( is_alert_now ) {
    big_led_fading();
  } else {  
    analogWrite( LED_ALARM, 0);
  }
}

void big_led_fading() {
  analogWrite( LED_ALARM, brightness_values[big_led_cur_bright]);
  if ( big_led_bright_direction ) {
    if ( big_led_cur_bright <= 0 ) {
      big_led_bright_direction^=1;
    } else {
      big_led_cur_bright--;
    }
  } else {
    if ( big_led_cur_bright >= big_led_brightness_values_max ) {
      big_led_bright_direction^=1;
    } else {
      big_led_cur_bright++;
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

/*
void manual_brightness_adjustment(){
  char inChar[2] = {0};  // because strncpy/strncat needs \0-terminated 2nd argument
  bool display_brightness_set = false;
  bool big_led_brightness_set = false;
  
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
              big_led_brightness_values_max++;
              big_led_brightness_set = true;
              break;
    case 'a':
              big_led_brightness_values_max--;
              big_led_brightness_set = true;
              break;
    case 'w':
              display_brightness++;
              display_brightness_set = true;
              break;
    case 's': 
              display_brightness--;
              display_brightness_set = true;
              break;
    default:
              Serial.print("Unknown symbol \"");
              Serial.print( inChar[0] );
              Serial.println("\"");
              return;
  }
  
  if (  display_brightness_set ) {
    if ( display_brightness > 7 ) {
      display_brightness = 7;
    } else if ( display_brightness < 0 ) {
      display_brightness = 0;
    }
    display.setBrightness(display_brightness);
    display_brightness_set = false;
    Serial.print("Current display brightness ");
    Serial.print( display_brightness );
    Serial.println();
  }

  if ( big_led_brightness_set ) {
    if ( big_led_brightness_values_max > COUNT_BRIGHTNESS_VALUES -1 ) {
      big_led_brightness_values_max = COUNT_BRIGHTNESS_VALUES - 1;
    } else if ( big_led_brightness_values_max < 0 ) {
      big_led_brightness_values_max = 0;
    }
    // big_led_brightness_set = false;
    Serial.print("Current max brightness ");
    Serial.print( big_led_brightness_values_max );
    Serial.println();
  }
}
*/
