
void toggleMode() {

  switch (mode) {

    case MODE_NORMAL:
      break;

    case MODE_SET_CLOCK:
      rtc.adjust(DateTime(rtc.now().year(), rtc.now().month(), rtc.now().day(), hours, minutes, 0));
      break;
 
    case MODE_SET_ALARM:
      break;
 
    case MODE_ACTIVATE_ALARM:
      break;
    
  }

  mode++;
  if (mode == MODE_UNKNOWN) { mode = MODE_NORMAL; }
  clearClock();
  
}

void toggleBlipColour() {

  pipCounter++;
  if (pipCounter > PIP_COUNTER_MAX) { pipCounter = 0; }
  
}

uint32_t getColourPips_SetClock() {

  return (pipCounter <= PIP_COUNTER_HALF ? colourPips_SetClock : 0);
  
}

uint32_t getColourPips_SetAlarm() {

  return (pipCounter <= PIP_COUNTER_HALF ? colourPips_SetAlarm : 0);
  
}

boolean isButtonPressed() {

  buttonPressed = (digitalRead(RTC_BUTTON_PIN) == 0);
  if (buttonPressed) { delay(250); }

  return buttonPressed;
  
}

void clearButtonPressed() {

  buttonPressed = false;
  
}

boolean wasButtonPressed() {

  return buttonPressed;

}
