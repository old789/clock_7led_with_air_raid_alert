#define DEBUG_SERIAL  // because just "DEBUG" defined in PZEM004Tv30.h ( legacy :)
#define DBG_WIFI    // because "DEBUG_WIFI" defined in a WiFiClient library
#define DEBUG_TIME
// #define DEBUG_HTTP
// #define DEBUG_LIGHT

#if defined ( DBG_WIFI ) && not defined ( DEBUG_SERIAL )
#define DEBUG_SERIAL
#endif

#if defined ( DEBUG_TIME ) && not defined ( DEBUG_SERIAL )
#define DEBUG_SERIAL
#endif

#if defined ( DEBUG_HTTP ) && not defined ( DEBUG_SERIAL )
#define DEBUG_SERIAL
#endif

// Include the library
#include <TM1637Display.h>  // https://github.com/avishorp/TM1637
#include "uRTCLib.h"        // https://github.com/Naguissa/uRTCLib
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <WiFiClientSecureBearSSL.h>
#include <time.h>
#include <coredecls.h> // optional settimeofday_cb() callback to check on server
#include <EEPROM.h>
#include <SimpleCLI.h>  // https://github.com/SpacehuhnTech/SimpleCLI
#include "TickTwo.h"    // https://github.com/sstaub/TickTwo
#include <microDS18B20.h>   // https://github.com/GyverLibs/microDS18B20/
#include <ArduinoJson.h>       // https://arduinojson.org/

// Define the 4 digits display connections pins
#define CLK D5
#define DIO D3

// Define the RTC connections pins
#define SCL D6
#define SDA D7

// Define the alarm LED pin
#define LED_ALARM D8

// Define ports of buttons
#define   SWITCH_TO_CONSOLE_MODE  D1
#define   SWITCH_NO_ALARM_MODE    D2

#define     TICS_SHOW_DOTS    4  // ( interval when dots on, 1/10s )
#define     TICS_SHOW_ERR    20  // ( interval when show error sign, 1/10s )
#define     TICS_SHOW_TEMPERATURE    30  // ( interval when show temperature, 1/10s )
#define     MAX_ALLOWED_INPUT 127
#define     REGION_COUNT      26
#define     STARUP_DELAY_FOR_NTP  5 // minutes
#define     PAUSE_BEFORE_NTP_TIME_WILL_NO_VALID   24 * 3600   // 1 day
#define     DEBOUNCE_DELAY  20  // ( interval after turn switch, 1/10s )
#define     DELAY_MEASUREMENT_DS18B20  10 // ( 1/10s )

// #define     BIG_LED_MAX_BRIGHT  120
// #define     BIG_LED_BRIGHT_STEPS  40
// #define     BIG_LED_BRIGHT_STEP  BIG_LED_MAX_BRIGHT / BIG_LED_BRIGHT_STEPS
// #define     BIG_LED_MIN_BRIGHT  1
// #define     TICS_BIG_LED_PAUSE  20
#define  COUNT_BRIGHTNESS_VALUES 55
const uint8_t brightness_values[COUNT_BRIGHTNESS_VALUES] = { 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 26, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 117, 129, 142, 156, 172, 189, 208, 229, 254 };

#define     AIR_RAID_API_URL    "http://ubilling.net.ua/aerialalerts/"

//#define   NOP __asm__ __volatile__ ("nop\n\t")

// Create a display object of type TM1637Display
TM1637Display display = TM1637Display(CLK, DIO);

// uRTCLib rtc;
uRTCLib rtc(0x68);

// DS18B20 sensor
MicroDS18B20<2> thermometer;

// Create CLI Object
SimpleCLI cli;

void pulse();
void update_time();
void check_air_raid_api();
void check_system();
void check_is_sntp_valid();
void time_is_set();

// Create timers object
TickTwo timer1( pulse, 100);  // 0.1s
TickTwo timer2( update_time, 1000);   // 1s
TickTwo timer3( check_air_raid_api, 5 * 1000);   // 5s
TickTwo timer4( check_system, 30 * 1000 );  // 30s
//TickTwo timer5( check_is_sntp_valid, 3 * 3600 * 1000);  // 3 hours
TickTwo timer5( check_is_sntp_valid, 20 * 60 * 1000);  // test

// Create an array that turns all segments ON
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};

// Create an array that turns all segments OFF
const uint8_t allOFF[] = {0x00, 0x00, 0x00, 0x00};

// Create an array that sets individual segments per digit to display the word "Err"
const uint8_t err[] = {
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,            // E
  SEG_E | SEG_G,                                    // r
  SEG_E | SEG_G                                     // r
};

// Create an array that sets individual segments per digit to display the word "Con" ("Console")
const uint8_t con[] = {
  SEG_A | SEG_D | SEG_E | SEG_F,                    // C
  SEG_C | SEG_D | SEG_E | SEG_G,                    // o
  SEG_C | SEG_E | SEG_G                             // n
};

// Create an array that sets individual segments per digit to display the word "noA" ("no Answer')
const uint8_t noa[] = {
  SEG_C | SEG_E | SEG_G,                            // n
  SEG_C | SEG_D | SEG_E | SEG_G,                    // o
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G     // A
};

// Create an array that sets individual segments per digit to display the word "noC" ("not Connected')
const uint8_t noc[] = {
  SEG_C | SEG_E | SEG_G,                            // n
  SEG_C | SEG_D | SEG_E | SEG_G,                    // o
  SEG_A | SEG_D | SEG_E | SEG_F,                    // C
};

// Create an array that sets individual segments per digit to display the word "not" ("not time')
const uint8_t noT[] = {
  SEG_C | SEG_E | SEG_G,                            // n
  SEG_C | SEG_D | SEG_E | SEG_G,                    // o
  SEG_D | SEG_E | SEG_F | SEG_G,                    // t
};

// Create an array that sets individual segments per digit to display '----'
const uint8_t notime[] = {
  SEG_G,                            // -
  SEG_G,                            // -
  SEG_G,                            // -
  SEG_G                             // -
};

// Create an array that sets individual segments per digit to display '^v^v'
const uint8_t wave[2][4] = {{
  SEG_F | SEG_A | SEG_B,                            // ^
  SEG_C | SEG_D | SEG_E,                            // v
  SEG_F | SEG_A | SEG_B,                            // ^
  SEG_C | SEG_D | SEG_E                             // v
},{
  SEG_C | SEG_D | SEG_E,                            // v
  SEG_F | SEG_A | SEG_B,                            // ^
  SEG_C | SEG_D | SEG_E,                            // v
  SEG_F | SEG_A | SEG_B                             // ^
}};

// Create an array for the temperature display
uint8_t temp_segments[] = {
  SEG_G,                          // Minus
  SEG_G,                          // Stub
  SEG_G,                          // Stub
  SEG_A | SEG_B | SEG_F | SEG_G   // Degree symbol
};

time_t now;                       // this are the seconds since Epoch (1970) - UTC
tm tm;                            // the structure tm holds time information in a more convenient way
time_t last_sntp_sync = 0;

//uint8_t secs = 0;
uint8_t mins = 0;
uint8_t hours = 0;

bool dots_display = false;
unsigned int tics_show_dots = 0;
unsigned int tics_show_not = 0;
unsigned int tics_show_noa = 0;
unsigned int tics_show_noc = 0;
unsigned int tics_show_t = 0;
unsigned int tics_debounce = 0;
unsigned int tics_before_temperature_ready = DELAY_MEASUREMENT_DS18B20;
// int16_t tics_bright_step = 0;
bool enable_cli = false;
bool is_sntp_valid = false;
bool is_rtc_valid = false;
bool is_air_raid_api_ok = false;
bool is_alert_now = false;
bool is_temperature_ready = false;
bool show_noa = false;
bool show_noc = false;
bool show_not = false;
bool show_t = false;
int16_t big_led_cur_bright = 0;
// uint8_t big_led_cur_pause = 0;
uint8_t big_led_bright_direction = 0;
uint8_t big_led_brightness_values_max = COUNT_BRIGHTNESS_VALUES - 1;
int light_sensor_data[16] = { 0 };
uint8_t l_data_cur = 0;
int illuminance = 0;
uint16_t temperature = 0;
// int16_t display_brightness = 5;
#ifdef DEBUG_LIGHT
unsigned int tics_show_illuminance = 0;
bool show_illuminance = false;
#endif

const char* region_name[REGION_COUNT] = {
  "Відключено",               // 0
  "Вінницька область",        // 1
  "Волинська область",        // 2
  "Дніпропетровська область", // 3
  "Донецька область",         // 4
  "Житомирська область",      // 5
  "Закарпатська область",     // 6 
  "Запорізька область",       // 7
  "Івано-Франківська область",// 8  
  "Київська область",         // 9
  "Кіровоградська область",   // 10
  "Луганська область",        // 11
  "Львівська область",        // 12
  "Миколаївська область",     // 13
  "Одеська область",          // 14
  "Полтавська область",       // 15
  "Рівненська область",       // 16
  "Сумська область",          // 17
  "Тернопільська область",    // 18
  "Харківська область",       // 19
  "Херсонська область",       // 20
  "Хмельницька область",      // 21
  "Черкаська область",        // 22
  "Чернівецька область",      // 23
  "Чернігівська область",     // 24
  "м. Київ"                   // 25
};

// EEPROM data
uint16_t mark = 0x55aa;
uint16_t region = 0;
uint16_t poll_interval = 60;  // minutes
char ssid[33];
char passw[65];
char host[33];
char tzdata[129];

#define PT_REGION       sizeof(mark)
#define PT_POLL         PT_REGION+sizeof(region)
#define PT_SSID         PT_POLL + sizeof(poll_interval)
#define PT_PASSW        PT_SSID + sizeof(ssid)
#define PT_HOST         PT_PASSW + sizeof(passw)
#define PT_TZDATA       PT_HOST + sizeof(host)
#define PT_CRC          PT_TZDATA + sizeof(tzdata)
#define SIZE_EEPROM     PT_TZDATA + sizeof(tzdata) - 1 // PT_CRC d'not count

// CLI Commands
Command cmdPoll;
Command cmdSsid;
Command cmdPassw;
Command cmdShow;
Command cmdHost;
Command cmdRegion;
Command cmdTZdata;
Command cmdSave;
Command cmdReboot;
Command cmdList;
Command cmdHelp;

void setup() {
  //pinMode( LED_BUILTIN, OUTPUT );
  pinMode( LED_ALARM, OUTPUT );
  pinMode( SWITCH_TO_CONSOLE_MODE, INPUT_PULLUP );
  pinMode( SWITCH_NO_ALARM_MODE, INPUT_PULLUP );
  digitalWrite( LED_ALARM, LOW );

#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Debug serial mode started"));
#endif

  EEPROM.begin(1024);
  
  // Set the brightness to 4 (0=dimmest 7=brightest)
	display.setBrightness( 4 );
	// Set all segments ON
	display.setSegments(allON);
  delay(2000);
  display.clear();

  URTCLIB_WIRE.begin( SDA, SCL );

  if ( rtc.lostPower() ) {
    display.setSegments(err,3,1);
    rtc.lostPowerClear();
    is_rtc_valid = false;
    delay(10000);
  } else {
    is_rtc_valid = true;
    rtc.refresh();
    display.showNumberDec(rtc.day(),true,2,1);
    delay(1000);
    display.clear();
    display.showNumberDec(rtc.month(),true,2,1);
    delay(1000);
    display.showNumberDec(2000 + rtc.year());
    delay(1000);
  }
  
  if ( digitalRead(SWITCH_TO_CONSOLE_MODE) == LOW ) {
#if not defined DEBUG_SERIAL
    Serial.begin(115200);
    delay(1000);
#endif
    enable_cli = true;
    display.clear();
    display.setSegments(con,3,1);
    Serial.println(F("CommandLine Mode"));
    (void)eeprom_read();
    SetSimpleCli();
  } else {
    if ( eeprom_read() ) { 
      configTime(tzdata, host);
      settimeofday_cb(time_is_set); // callback if time was sent
      wifi_init();
    } else {
      display.clear();
      display.setSegments(err,3,1);
      delay(3000);
    }
    thermometer.requestTemp();
    timer1.start();
    timer2.start();
    timer3.start();
    timer4.start();
    timer5.start();
  }

}

void loop() {
 if (enable_cli) {
    loop_cli_mode();
  }else{
    loop_usual_mode();
  }
}

void loop_usual_mode(){
  timer1.update();
  timer2.update();
  timer3.update();
  timer4.update();
  timer5.update();
}

void pulse() {
  
  if ( digitalRead(SWITCH_TO_CONSOLE_MODE) == LOW ) {
    if ( tics_debounce++ > DEBOUNCE_DELAY ) {
#ifdef DEBUG_SERIAL
      Serial.println(F("reboot to console mode"));
#endif
      ESP.restart();
    }
  }

  // manual_brightness_adjustment();

  ambient_light_sensor();
  
  led_alert();
  
  if ( ! is_temperature_ready ) {
    temperature_get_ready();
  }

/*
#ifdef DEBUG_LIGHT
  if ( display_illuminance() ) {
    return;
  }
#endif
*/

  // show "not Connected" to WiFi error
  if ( show_info( &tics_show_noc, noc, &show_noc )) {
    return;
  }
  
  // show air raid API "not Answered" error
  if ( show_info( &tics_show_noa, noa, &show_noa )) {
    return;
  }
  
  // show no NTP time error
  if ( show_info( &tics_show_not, noT, &show_not )) {
    return;
  }
  
  // show temperature
  if ( show_temperature() ) {
    return;
  }
  
  if ( is_sntp_valid or is_rtc_valid ) {
    if ( tics_show_dots > 0) {
      if ( ! dots_display ) {
        dots_display = true;
        display.showNumberDecEx( hours * 100 + mins, 0b01000000, true, 4, 0);
      }
      tics_show_dots--;
    }else{
      if ( dots_display ) {
        display.showNumberDecEx( hours * 100 + mins, 0, true, 4, 0);
        dots_display = false;
      }
    }
  } else {
    display.setSegments(notime);
  }

}

void update_time() {
  if ( is_sntp_valid ) {
    time(&now);                       // read the current time
    localtime_r(&now, &tm);           // update the structure tm with the current time
    hours = tm.tm_hour;
    mins = tm.tm_min;
  } else if ( is_rtc_valid ) { 
      rtc.refresh();
      hours = rtc.hour();
      mins = rtc.minute();
  }
  tics_show_dots = TICS_SHOW_DOTS;
#ifdef DEBUG_LIGHT
  tics_show_illuminance = 5;
#endif
}

void check_system() {
  if ( ! is_sntp_valid ) {
    tics_show_not = TICS_SHOW_ERR;
  }
  if ( WiFi.status() != WL_CONNECTED ) {
    tics_show_noc = TICS_SHOW_ERR;
  }
  if ( ! is_air_raid_api_ok ) {
    tics_show_noa = TICS_SHOW_ERR;
  }
  
  thermometer.requestTemp();
  tics_before_temperature_ready = DELAY_MEASUREMENT_DS18B20;
  is_temperature_ready = false;

  if ( ( tics_debounce > 0 )  and ( digitalRead(SWITCH_TO_CONSOLE_MODE) == HIGH ) ) {
    tics_debounce = 0;
#ifdef DEBUG_SERIAL
    Serial.println(F("tics_debounce cleared"));
#endif
  }
}

bool show_info( unsigned int *tics, const uint8_t *info, bool *show ) {
  if ( *tics > 0 ) {
    if ( ! *show ) {
      display.clear();
      display.setSegments(info,3,1);
      *show = true;
    }
    (*tics)--;
    return(true);
  } else if ( *show ) {
    *show = false;
  }
  return(false);
}

void temperature_get_ready(){
  if ( tics_before_temperature_ready > 0 ) {
    tics_before_temperature_ready--;
  } else {
    if ( thermometer.readTemp() ) {
      is_temperature_ready = true;
      tics_show_t = TICS_SHOW_TEMPERATURE;
    }
  }
}

bool show_temperature() {
  int temperature;
  uint16_t t1;
  uint8_t hdig;
  uint8_t ldig;
  
  if ( ! is_temperature_ready ) {
    return(false);
  }
  
  if ( tics_show_t <= 0 ) {
    if ( show_t ) {
      show_t = false;
    }
    return(false);
  }
  
  if ( show_t ) {
    tics_show_t--;
    return(true);
  }
  
  temperature = thermometer.getTempInt();
  t1 = abs( temperature );
  hdig = t1 / 10;
  ldig = t1 % 10;
  temp_segments[1] =  display.encodeDigit(hdig);
  temp_segments[2] =  display.encodeDigit(ldig);
  if ( temperature >= 0 ) {
    display.clear();
    display.setSegments(temp_segments+sizeof(uint8_t),3,1);
  }else{
    display.setSegments(temp_segments);
  }
  show_t = true;
  return(true);
}

#ifdef DEBUG_LIGHT
bool display_illuminance(){
  if ( tics_show_illuminance > 0 ) {
    if ( ! show_illuminance ) {
      display.clear();
      display.showNumberDecEx( illuminance, 0, true, 4, 0);
      show_illuminance = true;
    }
    tics_show_illuminance--;
    return(true);
  } else if ( show_illuminance ) {
    show_illuminance = false;
  }
  return(false);
}
#endif


void check_air_raid_api(){
  if ( digitalRead(SWITCH_NO_ALARM_MODE) == LOW ) {
    is_air_raid_api_ok = true;
    is_alert_now = false;
    return;
  }
  
  is_air_raid_api_ok = false;

  if ( WiFi.status() != WL_CONNECTED ) {
#ifdef DEBUG_SERIAL
    Serial.println("[HTTP] WiFi not connected");
#endif
    return;
  }

  WiFiClient client;
  HTTPClient http;
  JsonDocument jroot;
  JsonDocument jfilter;

#ifdef DEBUG_HTTP
  Serial.println("[HTTP] begin...");
#endif

  if ( ! http.begin(client, AIR_RAID_API_URL)) {
#ifdef DEBUG_SERIAL
    Serial.println("[HTTP] Unable to connect");
#endif
    return;
  }

#ifdef DEBUG_HTTP
  Serial.println("[HTTP] send GET request...");
#endif
  int httpCode = http.GET();
#ifdef DEBUG_HTTP
  Serial.printf("[HTTP] GET done with code: %d\r\n", httpCode);
#endif

  if (httpCode < 0) {
#ifdef DEBUG_SERIAL
    Serial.printf("[HTTP] GET failed, error: %s\r\n", http.errorToString(httpCode).c_str());
#endif
    return;
  }
  
  if ( ! ( httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY ) ) {
    return;
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
    return;
#ifdef DEBUG_HTTP
  } else {
  // serializeJsonPretty(jroot, Serial);
    Serial.println("[HTTP] JSON deserialized successfully");
#endif
  }

  bool alert_state = jroot["states"][region_name[region]]["alertnow"];
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
  http.end();
  return;
}

void ambient_light_sensor() {
int light_sensor = analogRead(A0);
int sum = 0;

  light_sensor_data[l_data_cur++] = light_sensor;
  l_data_cur &= 0b00001111;
  for ( uint8_t i = 0; i < 16; i++ ){
    sum += light_sensor_data[i];
  }
  illuminance = (int)(sum / 16.0 + 0.5);
  if ( illuminance < 30) {
    display.setBrightness(0);
    big_led_brightness_values_max = 10;
  } else if ( illuminance < 50) {
    display.setBrightness(1);
    big_led_brightness_values_max = 12;
  } else if ( illuminance < 70) {
    display.setBrightness(3);
    big_led_brightness_values_max = 19;
  } else {
    display.setBrightness(6);
    big_led_brightness_values_max = COUNT_BRIGHTNESS_VALUES - 1;
  } 
}  