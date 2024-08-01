#define DEBUG_SERIAL  // because just "DEBUG" defined in PZEM004Tv30.h ( legacy :)
#define DBG_WIFI    // because "DEBUG_WIFI" defined in a WiFiClient library

#if defined ( DBG_WIFI ) && not defined ( DEBUG_SERIAL )
#define DEBUG_SERIAL
#endif

// Include the library
#include <TM1637Display.h>  // https://github.com/avishorp/TM1637
#include "uRTCLib.h"        // https://github.com/Naguissa/uRTCLib
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <WiFiClientSecureBearSSL.h>
#include <EEPROM.h>
#include <SimpleCLI.h>  // https://github.com/SpacehuhnTech/SimpleCLI
#include "TickTwo.h"    // https://github.com/sstaub/TickTwo
// #include <microDS18B20.h>   // https://github.com/GyverLibs/microDS18B20/

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

// Define intervals to display temperature and time 
// #define   TEMPERATURE_TIME    5
// #define   CLOCK_TIME          10
#define     TICS_SHOW_DOTS    4  // ( interval when dots on, 1/10s )

#define   NOP __asm__ __volatile__ ("nop\n\t")

// Create a display object of type TM1637Display
TM1637Display display = TM1637Display(CLK, DIO);

// uRTCLib rtc;
uRTCLib rtc(0x68);

// DS18B20 sensor
// MicroDS18B20<D4> sensor1;

// Create CLI Object
SimpleCLI cli;

void pulse();
void update_time();
void sync_ntp();

// Create timers object
TickTwo timer1( pulse, 100);
TickTwo timer2( update_time, 1000);
TickTwo timer3( sync_ntp, 3600*1000);

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

// Create an array for the temperature display
uint8_t temp_segments[] = {
  SEG_G,                          // Minus
  SEG_G,                          // Stub
  SEG_G,                          // Stub
  SEG_A | SEG_B | SEG_F | SEG_G   // Degree symbol
};

uint8_t secs = 0;
uint8_t mins = 0;
uint8_t hours = 0;

bool dots_display = false;
bool temp_display = false;
unsigned int tics_show_dots = 0;
// uint8_t temp_enable = 0;
// int temper = 0;
bool enable_cli = false;


void setup() {
  //pinMode( LED_BUILTIN, OUTPUT );
  pinMode( LED_ALARM, OUTPUT );
  pinMode( SWITCH_TO_CONSOLE_MODE, INPUT_PULLUP );
  pinMode( SWITCH_NO_ALARM_MODE, INPUT_PULLUP );
  digitalWrite( LED_ALARM, LOW );
  
  // Set the brightness to 5 (0=dimmest 7=brightest)
	display.setBrightness(5);
	// Set all segments ON
	display.setSegments(allON);
  delay(2000);
  display.clear();

  URTCLIB_WIRE.begin( SDA, SCL );
  // rtc.set(0, 12, 18, 3, 31, 7, 24); delay(1000);
  if ( rtc.lostPower() ) {
    display.setSegments(err,3,1);
    rtc.lostPowerClear();
    delay(10000);
  } else {
    rtc.refresh();
    display.showNumberDec(rtc.day(),true,2,1);
    delay(1000);
    display.clear();
    display.showNumberDec(rtc.month(),true,2,1);
    delay(1000);
    display.showNumberDec(2000 + rtc.year());
    delay(1000);
  }
  hours = rtc.hour();
  mins = rtc.minute();

  // Serial.begin(9600);
  
  timer1.start();
  timer2.start();
  timer3.start();

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
  // nothing
  NOP;
  NOP;
  NOP;
  NOP;
  NOP;
  NOP;
  NOP;
  NOP;
}

void pulse() {
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
}

void update_time() {
  rtc.refresh();
  hours = rtc.hour();
  mins = rtc.minute();
  tics_show_dots = TICS_SHOW_DOTS;
}

void sync_ntp() {
}

void set_rtc() {
  rtc.refresh();
  uint8_t sec = rtc.second();
  uint8_t day = rtc.day();
	uint8_t month = rtc.month();
	uint8_t year = rtc.year();
	uint8_t dayOfWeek = rtc.dayOfWeek();
  rtc.set( sec, mins, hours, dayOfWeek, day, month, year );
}

/*
void display_temp( int t ){
  uint16_t t1 = abs(t);
  uint8_t hdig = t1 / 10;
  uint8_t ldig = t1 % 10;
  
  temp_segments[1] =  display.encodeDigit(hdig);
  temp_segments[2] =  display.encodeDigit(ldig);
  if (  t >= 0 ) {
    display.setSegments(temp_segments+sizeof(uint8_t),3,1);
  }else{
    display.setSegments(temp_segments);
  }
}
*/
