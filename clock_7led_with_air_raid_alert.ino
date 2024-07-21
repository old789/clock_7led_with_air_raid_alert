#define USE_SERIAL
// Include the library
#include <ESP8266WiFi.h>
#include <TM1637Display.h>  // https://github.com/avishorp/TM1637
#include <RtcDS1302.h>      // https://github.com/Makuna/Rtc

// Define the 4 digits display connections pins
#define CLK D5
#define DIO D3

// Define ports of buttons
#define   SWITCH_TO_CONSOLE_MODE  D1
#define   SWITCH_NO_ALARM_MODE    D2


#define invDigitalRead( X )   ~digitalRead( X ) & HIGH

#define   NOP __asm__ __volatile__ ("nop\n\t")

// Create a display object of type TM1637Display
TM1637Display display = TM1637Display(CLK, DIO);

ThreeWire myWire(D6,D7,D8); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

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


void setup() {
  pinMode( LED_BUILTIN, OUTPUT );
  pinMode( SWITCH_TO_CONSOLE_MODE, INPUT_PULLUP );
  pinMode( SWITCH_NO_ALARM_MODE, INPUT_PULLUP );
#ifdef USE_SERIAL
  Serial.begin(9600);
  delay(2000);
  Serial.println("Serial initialised");
#endif
  // Set the brightness to 5 (0=dimmest 7=brightest)
	display.setBrightness(5);
#ifdef USE_SERIAL
  Serial.println("set display brightness");
#endif
// Set all segments ON
	display.setSegments(allON);
	delay(2000);
  display.clear();
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid()) {
#ifdef USE_SERIAL
    Serial.println("RTC lost confidence in the DateTime!");
#endif
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()){
#ifdef USE_SERIAL
    Serial.println("RTC was write protected, enabling writing now");
#endif
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()){
#ifdef USE_SERIAL
    Serial.println("RTC was not actively running, starting now");
#endif
    Rtc.SetIsRunning(true);
  }

#ifdef USE_SERIAL
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
        Serial.print(compiled.Hour());
        Serial.print(":");
        Serial.print(compiled.Minute());
        Serial.print(":");
        Serial.println(compiled.Second());
        Serial.print(now.Hour());
        Serial.print(":");
        Serial.print(now.Minute());
        Serial.print(":");
        Serial.println(now.Second());
#endif

}

void loop() {

  // Displays current time

  // rtc.refresh();
  // hours = rtc.hour();
  // mins = rtc.minute();
 /*
  secs++;
  if ( secs == 60 ) {
    mins++;
    secs = 0;
    if ( mins == 60 ) {
      hours++;
      mins = 0;
      if ( hours == 24 ) {
        hours = 0;
      }
    }
  }
 */
 Rtc.GetDateTime();
 RtcDateTime now = Rtc.GetDateTime();
 hours = now.Hour();
 mins = now.Minute();
 secs = now.Second();

#ifdef USE_SERIAL
        Serial.print(now.Hour());
        Serial.print(":");
        Serial.print(now.Minute());
        Serial.print(":");
        Serial.println(now.Second());
#endif
  dots_display ^= true;
  
  if ( dots_display ) {
    digitalWrite( LED_BUILTIN, HIGH );
    display.showNumberDecEx( hours * 100 + mins, 0b01000000, true, 4, 0);
  } else {
    display.showNumberDecEx( hours * 100 + mins, 0, true, 4, 0);
    digitalWrite( LED_BUILTIN, LOW );
  }
  
  // nothing
  NOP;
  NOP;
  NOP;
  NOP;

  delay(1000);
}

/*
void set_rtc() {
  rtc.refresh();
  uint8_t sec = rtc.second();
  uint8_t day = rtc.day();
	uint8_t month = rtc.month();
	uint8_t year = rtc.year();
	uint8_t dayOfWeek = rtc.dayOfWeek();
  rtc.set( sec, mins, hours, dayOfWeek, day, month, year );
}
*/
