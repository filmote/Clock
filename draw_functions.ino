void clearClock() {

  for (uint16_t iPixels = 0; iPixels < strip.numPixels(); iPixels++) {

      strip.setPixelColor(iPixels, 0);

  }
    
}

void drawClock(uint8_t hours, uint8_t minutes, uint32_t colourHours, uint32_t colourMinutes, uint32_t colourSeconds, uint32_t colourPips) {

  int mi = minutes / 2.5;

  mSecs_Then = millis();
  
  do {

    if (wasButtonPressed()) { break;}
    
    for (uint16_t iPixels = 0; iPixels < strip.numPixels(); iPixels++) {

      if (isButtonPressed()) { toggleMode(); break;}


      // Draw pips ..

      if (SHOW_PIPS) {

        strip.setPixelColor(0, colourPips);
        strip.setPixelColor(strip.numPixels() * 0.25, colourPips);
        strip.setPixelColor(strip.numPixels() * 0.50, colourPips);
        strip.setPixelColor(strip.numPixels() * 0.75, colourPips);

      }

      
      // If evening then add second pips at top of clock ..

      if (SHOW_PM_INDICATOR && SHOW_PIPS && hours >= 12) {
      
        strip.setPixelColor(strip.numPixels() - 1, colourPips);
        strip.setPixelColor(1, colourPips);

      }

      if (SHOW_PM_INDICATOR && !SHOW_PIPS && hours >= 12) {
      
        strip.setPixelColor(0, colourPips);

      }


      // Draw seconds ..

      if (SHOW_SECONDS) {

        if (SHOW_PIPS) {
  
          if (iPixels - 1 != 0 && iPixels - 1 != strip.numPixels() * 0.25 && iPixels - 1 != strip.numPixels() * 0.50 && iPixels - 1 != strip.numPixels() * 0.75) {
            strip.setPixelColor((iPixels > 0 ? iPixels - 1: strip.numPixels() - 1), 0);
          }
          
        }
        else {
  
          strip.setPixelColor((iPixels > 0 ? iPixels - 1: strip.numPixels() - 1), 0);
  
        }
        
        strip.setPixelColor(iPixels, colourSeconds);
  
      }


      // Draw minutes ..
      
      strip.setPixelColor((mi == 0 ? strip.numPixels() - 1 : mi - 1), colourMinutes);
      strip.setPixelColor(mi, colourMinutes);
      strip.setPixelColor((mi == strip.numPixels() - 1 ? 0 : mi + 1), colourMinutes);
      

      // Draw hour ..
      
      strip.setPixelColor(((hours >= 12 ? hours - 12 : hours) * 2) + (minutes > 30 ? 1 : 0), colourHours);
      strip.show();
/*
      mSecs_Now = millis();
//      Serial.println((mSecs_Now - mSecs_Then));

      if ((mSecs_Now - mSecs_Then) < 45) {
      */
        delay(DELAY_PLUS_1);
      /*
      }
      else {
    
        delay(DELAY);

      }

      mSecs_Then = mSecs_Now;
*/
    } 
    
  } while (rtc.now().second() != 59);


  while (!wasButtonPressed() && rtc.now().second() != 0) {
    delay(DELAY);
  }  
  
}

void drawSetClock(uint8_t hours, uint8_t minutes, uint32_t colourHours, uint32_t colourMinutes, uint32_t colourPips) {

  int mi = minutes / 2.5;
  
  for (uint16_t iPixels = 0; iPixels < strip.numPixels(); iPixels++) {
  
    strip.setPixelColor(iPixels, 0);
  
  }
  
  
  // Draw pips ..
  
  if (SHOW_PIPS) {
  
    strip.setPixelColor(0, colourPips);
    strip.setPixelColor(strip.numPixels() * 0.25, colourPips);
    strip.setPixelColor(strip.numPixels() * 0.50, colourPips);
    strip.setPixelColor(strip.numPixels() * 0.75, colourPips);
  
  }
  
  // Always show PM indicator when setting time or alarm ..
  
  if (SHOW_PIPS && hours >= 12) {
  
    strip.setPixelColor(strip.numPixels() - 1, colourPips);
    strip.setPixelColor(1, colourPips);
  
  }
    
  
  // Draw minutes and hours ..
  
  strip.setPixelColor((mi == 0 ? strip.numPixels() - 1 : mi - 1), colourMinutes);
  strip.setPixelColor(mi, colourMinutes);
  strip.setPixelColor((mi == strip.numPixels() - 1 ? 0 : mi + 1), colourMinutes);
  strip.setPixelColor(((hours >= 12 ? hours - 12 : hours) * 2) + (minutes > 30 ? 1 : 0), colourHours);
  strip.show();
   
}

void drawActivateAlarm(uint32_t colourActive, uint32_t colourInactive) {

  for (uint16_t iPixels = 0; iPixels < strip.numPixels(); iPixels++) {
  
    strip.setPixelColor(iPixels, 0);

  }

  if (alarmSet) {
      
    for (uint16_t iPixels = 0; iPixels < (strip.numPixels() / 2); iPixels++) {
    
      strip.setPixelColor(iPixels, colourActive);
  
    }

  }
  else {
    
    for (uint16_t iPixels = (strip.numPixels() / 2); iPixels < strip.numPixels(); iPixels++) {
    
      strip.setPixelColor(iPixels, colourInactive);
  
    }
    
  }
  
  strip.show();
  
}

void drawAlarm(uint8_t wait) {
  
  uint16_t i, j;

  for( j=0; j<256; j++) {
    
    if (wasButtonPressed()) { mode = MODE_NORMAL; break;}
    
    for (i=0; i<strip.numPixels(); i++) {
      if (isButtonPressed()) { mode = MODE_NORMAL; break;}
      strip.setPixelColor(i, getColourCode((i+j) & 255));
    }
    
    strip.show();
    delay(wait);
  
  }

  clearClock();
  
}

uint32_t getColourCode(byte number) {

  number = 255 - number;
  
  if(number < 85) {
    return strip.Color(255 - number * 3, 0, number * 3);
  }

  if(number < 170) {
    number -= 85;
    return strip.Color(0, number * 3, 255 - number * 3);
  }
  
  number -= 170;
  
  return strip.Color(number * 3, 255 - number * 3, 0);
  
}
