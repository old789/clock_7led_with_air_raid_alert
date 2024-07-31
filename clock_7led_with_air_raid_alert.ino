// Include the library
#include <TM1637Display.h>  // https://github.com/avishorp/TM1637
#include <neotimer.h>       // https://github.com/jrullan/neotimer
#include "uRTCLib.h"        // https://github.com/Naguissa/uRTCLib
// #include <avr/wdt.h>
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

// #define invDigitalRead( X )   ~digitalRead( X ) & HIGH

#define   NOP __asm__ __volatile__ ("nop\n\t")

// Create a display object of type TM1637Display
TM1637Display display = TM1637Display(CLK, DIO);

// uRTCLib rtc;
uRTCLib rtc(0x68);

// DS18B20 sensor
// MicroDS18B20<D4> sensor1;

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

uint8_t temp_segments[] = {
  SEG_G,                          // Minus
  SEG_G,                          // Stub
  SEG_G,                          // Stub
  SEG_A | SEG_B | SEG_F | SEG_G   // Degree symbol
};

uint8_t secs = 0;
uint8_t mins = 0;
uint8_t hours = 0;

// bool btn_min = false;
// bool first_pass_min = true;
// uint8_t i_min = 0;
// bool btn_hrs = false;
// bool first_pass_hrs = true;
// uint8_t i_hrs = 0;
bool dots_display = false;
// bool temp_display = false;
// uint8_t clock_enable = 0;
// uint8_t temp_enable = 0;
// int temper = 0;

Neotimer maintimer = Neotimer( 1000 );
Neotimer secondtimer = Neotimer( 400 );
// Neotimer btn_reset_tmr = Neotimer();
// Neotimer btn_press_set_h_tmr = Neotimer();
// Neotimer btn_press_set_m_tmr = Neotimer();
// Neotimer btn_hold_set_h_tmr = Neotimer();
// Neotimer btn_hold_set_m_tmr = Neotimer();
// Neotimer btn_release_set_h_tmr = Neotimer();
// Neotimer btn_release_set_m_tmr = Neotimer();

/* 
 * без скидання MCUSR буде bootloop після спрацювання watchdog після включення живлення на чіпах PA & PB
 * що цікаво, після скидання сигналом RESET або у випадку чіпів P, такого ефекту нема.
 * Взагалі-то досить код із цієї функції помістити в функцію setup, але мануал на чіп радить так.
 */
 
// void clr_mcusr(void) __attribute__((naked)) __attribute__((section(".init3")));

// void clr_mcusr(void){
  // MCUSR = 0;
  // wdt_disable();
// }

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
  URTCLIB_WIRE.begin( SDA, SCL );
	delay(2000);
  // rtc.set(0, 12, 18, 3, 31, 7, 24); delay(1000);
  display.clear();
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
}

void loop() {
  // Displays current time ( with dots ) or temperature
  if ( maintimer.repeat() ) {

 /*     if ( temp_display ) {
        temp_enable++;
        if ( temp_enable > TEMPERATURE_TIME ) {
          temp_display = false;
          temp_enable = 0;
        }
      } else {
        clock_enable++;
        if ( clock_enable ==  CLOCK_TIME ){
          sensor1.requestTemp();
        }
        if ( clock_enable > CLOCK_TIME ) {
          temp_display = true;
          clock_enable = 0;
        }
      }
      if ( temp_display ) { // displays temperature
        temper = sensor1.getTemp();
        if ( temp_enable == 0 ) {
          display.clear();
        }
        display_temp( temper );
      } else { // displays time
  */
        rtc.refresh();
        hours = rtc.hour();
        mins = rtc.minute();
        display.showNumberDecEx( hours * 100 + mins, 0b01000000, true, 4, 0);
  //    }
     
    dots_display = true;
    // digitalWrite( LED_BUILTIN, HIGH );
    secondtimer.start();
    // wdt_reset();
  }
  
  // Dots off for blink
  if ( secondtimer.done() ){
    display.showNumberDecEx( hours * 100 + mins, 0, true, 4, 0);
    dots_display = false;
    // digitalWrite( LED_BUILTIN, LOW );
    secondtimer.reset();
  }

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
