#include <Adafruit_NeoPixel.h>
#include <Time.h>  
#include <Wire.h>
#include "RTClib.h"
#include <Encoder.h>
#include <EEPROM.h>

#define SHOW_PM_INDICATOR false
#define SHOW_PIPS true
#define SHOW_SECONDS true

#define NEO_PIN 7
#define RTC_A_PIN 3
#define RTC_B_PIN 4
#define RTC_BUTTON_PIN 5

#define MINUTES_IN_HOUR 60
#define SECONDS_IN_MINUTES 60
#define DELAY 43
#define DELAY_PLUS_1 44

#define MODE_NORMAL 0
#define MODE_SET_CLOCK 1
#define MODE_SET_ALARM 2
#define MODE_ACTIVATE_ALARM 3
#define MODE_UNKNOWN 4
#define MODE_ALARM_RINGING 5

#define PIP_COUNTER_MAX 4
#define PIP_COUNTER_HALF 2
#define BLINK_DELAY 150

#define MEM_ALARM_ON 0
#define MEM_ALARM_HOURS 1
#define MEM_ALARM_MINUTES 2

Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, NEO_PIN, NEO_GRB + NEO_KHZ800);
RTC_DS1307 rtc;
Encoder myEnc(RTC_A_PIN, RTC_B_PIN);

int mode = MODE_NORMAL; 
int hours = 0;
int minutes = 0;
int hours_alarm = 0;
int minutes_alarm = 0;
int pipCounter = 0;

long oldPosition  = -999;
long lastUpdateMillis = 0;
boolean buttonPressed = false;
boolean alarmSet = false;

uint32_t colourHours = strip.Color(50, 0, 0);
uint32_t colourMinutes = strip.Color(0, 50, 0);
uint32_t colourSeconds = strip.Color(0, 0, 50);
uint32_t colourPips = strip.Color(15, 15, 15);

uint32_t colourHours_SetClock = strip.Color(30, 0, 0);
uint32_t colourMinutes_SetClock = strip.Color(0, 30, 0);
uint32_t colourPips_SetClock = strip.Color(0, 8, 8);

uint32_t colourHours_SetAlarm = strip.Color(30, 0, 0);
uint32_t colourMinutes_SetAlarm = strip.Color(0, 30, 0);
uint32_t colourPips_SetAlarm = strip.Color(8, 8, 0);

uint32_t colour_ActivateAlarm_Active = strip.Color(0, 50, 0);
uint32_t colour_ActivateAlarm_Inactive = strip.Color(50, 0, 0);

unsigned long mSecs_Then;
unsigned long mSecs_Now;

void setup() {

  Serial.begin(115200);

  if (!rtc.begin()) {
    
    Serial.println("Couldn't find RTC");
    while (1);
    
  }

  if (!rtc.isrunning()) {
    
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  }

  hours = rtc.now().hour();
  minutes = rtc.now().minute();
  
  strip.setBrightness(32);
  strip.begin();
  strip.show(); 

  pinMode(RTC_BUTTON_PIN, INPUT_PULLUP);


  // Retrieve alarm settings from EEPROM ..

  alarmSet = (EEPROM.read(MEM_ALARM_ON) == 1);
  hours_alarm = EEPROM.read(MEM_ALARM_HOURS);
  minutes_alarm = EEPROM.read(MEM_ALARM_MINUTES);

  if (hours_alarm > 23)   { hours_alarm = 0; }
  if (minutes_alarm > 60) { minutes_alarm = 0; }
  
}

void loop() {


  // Should the alarm be set?

  if (alarmSet && hours == hours_alarm && minutes == minutes_alarm && rtc.now().second() == 0) {

    mode = MODE_ALARM_RINGING;
    
  }


  // Has the button been pressed ?
  
  if (isButtonPressed()) { toggleMode(); clearButtonPressed(); }


  // Has the dial been turned ?
  
  long newPosition = myEnc.read();

  if (newPosition != oldPosition) {

    if (mode == MODE_SET_CLOCK) {

        if (newPosition < oldPosition) { hours++; if (hours >= 24) { hours = 0; } }
        if (newPosition > oldPosition) { minutes = minutes - 3; if (minutes < 0) { minutes = minutes + 60; } }
      
    }

    if (mode == MODE_SET_ALARM) {

        if (newPosition < oldPosition) { hours_alarm++; if (hours_alarm >= 24) { hours_alarm = 0; } }
        if (newPosition > oldPosition) { minutes_alarm = minutes_alarm - 3; if (minutes_alarm < 0) { minutes_alarm = minutes_alarm + 60; } }

        EEPROM.write(MEM_ALARM_HOURS, hours_alarm);
        EEPROM.write(MEM_ALARM_MINUTES, minutes_alarm);

    }

    if (mode == MODE_ACTIVATE_ALARM) {

      alarmSet = !alarmSet;
      EEPROM.write(MEM_ALARM_ON, (alarmSet ? 1 : 0));
      
    }

    oldPosition = newPosition;

  }
   
  switch (mode) {
    
    case MODE_NORMAL:
      hours = rtc.now().hour();
      minutes = rtc.now().minute();
      drawClock(hours, minutes, colourHours, colourMinutes, colourSeconds, colourPips); 
      hours = rtc.now().hour();
      minutes = rtc.now().minute();
      break;

    case MODE_SET_CLOCK:
      toggleBlipColour();
      drawSetClock(hours, minutes, colourHours_SetClock, colourMinutes_SetClock, getColourPips_SetClock()); 
      delay(BLINK_DELAY);
      break;

    case MODE_SET_ALARM:
      toggleBlipColour();
      drawSetClock(hours_alarm, minutes_alarm, colourHours_SetAlarm, colourMinutes_SetAlarm, getColourPips_SetAlarm()); 
      delay(BLINK_DELAY);
      break;

    case MODE_ACTIVATE_ALARM:
      drawActivateAlarm(colour_ActivateAlarm_Active, colour_ActivateAlarm_Inactive); 
      delay(BLINK_DELAY);
      break;

    case MODE_ALARM_RINGING:
      drawAlarm(20); 
      delay(BLINK_DELAY);
      break;

  }
    
  clearButtonPressed();
  
}


