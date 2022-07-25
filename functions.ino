void rotationISR() {
  rotary.handleRotationCount();
}

void switchISR() {
  rotary.handleSwitchState();
}

void eepromWriteInt(int adress, int value) {
  byte one = (value & 0xFF);
  byte ten = ((value >> 8) & 0xFF);
  EEPROM.write(adress + 1, ten);
  EEPROM.write(adress, one);
}

int eepromReadInt(int adress) {
  int ten = EEPROM.read(adress + 1);
  int one = EEPROM.read(adress);
  return (one & 0xFF) + ((ten & 0xFF) << 8);
}

void showRunPage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" In Temp");
  lcd.setCursor(0, 1);
  lcd.print("Set:");

//  lcd.setCursor(11, 1);
//  lcd.print("Pre:");

  lcd.setCursor(0, 2);
  lcd.print("Pre:");

  lcd.setCursor(0, 3);
  lcd.print("Time: ");

  lcd.setCursor(6, 3);
  lcd.print(runTime/60);
  lcd.print(" H ");
  lcd.print(runTime%60);
  lcd.print(" M ");

  updateDisSetTempIn(setTempIn);
  updateDisPreTempIn(tempIn);
//  updateDisPreTempOut(tempOut);
}

void updateDisSetTempIn(float setTempIn) {
  lcd.setCursor(4, 1);
  lcd.print("    ");
  lcd.setCursor(4, 1);
  lcd.print(setTempIn, 1);
}

void updateDisPreTempIn(float tempIn) {
  lcd.setCursor(4, 2);
  lcd.print("    ");
  lcd.setCursor(4, 2);
  lcd.print(tempIn, 1);
}

void updateDisPreTempOut(float tempOut) {
  lcd.setCursor(15, 1);
  lcd.print("    ");
  lcd.setCursor(15, 1);
  lcd.print(tempOut, 1);
}

float restrictValue(float value, float minValue, float maxValue) {
  return min(max(value, minValue), maxValue);
}

void getRunParams() {
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print(" In Temp");

  lcd.setCursor(0, 1);
  lcd.print("Set:");
  lcd.setCursor(4, 1);
  lcd.print(setTempIn, 1);
  setTempIn = rotary.getValueInput([](int value) {
    lcd.setCursor(4, 1);
    lcd.print(value / 10.0, 1);
  }, setTempIn * 10, 0, tempMax * 10);

  if (String(setTempIn / 10.0) != String(preSetTempIn)) {
    eepromWriteInt(0, setTempIn);
    preSetTempIn = setTempIn / 10.0;
  }
  setTempIn /= 10.0;

  int runHours = runTime/60;
  int runMins = runTime%60;

  lcd.setCursor(0, 2);
  lcd.print("RunTime: ");
  lcd.setCursor(9, 2);
  lcd.print(runHours);
  lcd.setCursor(13, 2);
  lcd.print("H");
  
  runHours = rotary.getValueInput([](int value) {
    lcd.setCursor(9, 2);
    lcd.print("   ");
    lcd.setCursor(9, 2);
    lcd.print(value);
  }, runHours, 0, 255);

  lcd.setCursor(16, 2);
  lcd.print(runMins);
  lcd.setCursor(18, 2);
  lcd.print("M");

  runMins = rotary.getValueInput([](int value) {
    lcd.setCursor(15, 2);
    lcd.print("  ");
    lcd.setCursor(15, 2);
    lcd.print(value);
  }, runMins, 0, 59);

  runTime = runHours * 60 + runMins;

  if (runTime != preRunTime) {
    eepromWriteInt(4, runTime);
  }

  lcd.setCursor(8, 3);
  lcd.print("<OK?>");
  while (!rotary.isPressed());
}

void updateTime(unsigned long gapTime) {
  passedTime += gapTime;
  /* 'passedTime' is in millis */
  if (passedTime > (runTime * 60000)) {
    timer.setTimerOff();
    beep(3, 500);
    stopProgram();
  }
  unsigned long remainingTime = runTime * 60000 - passedTime;
  int remainingHours = remainingTime / 60000 / 60;
  int remainingMins = (remainingTime / 60000) - (remainingHours * 60);
  lcd.setCursor(6, 3);
  lcd.print(remainingHours);
  lcd.print(" H ");
  lcd.print(remainingMins);
  lcd.print(" M ");
}

void servoPress() {
  for (uint8_t pos = 100; pos <= 150; ++pos) {
    servoWrite(pos);
    timeMillis = millis() + 15;
    while(millis() < timeMillis){
      timer.run();
    }
  }
  for (uint8_t pos = 150; pos >= 100; --pos) {
    servoWrite(pos);
    timeMillis = millis() + 15;
    while(millis() < timeMillis){
      timer.run();
    }
  }
}

void servoInit() {
  /* For pin 9 */
  pinMode(9, OUTPUT);
  TCCR1A = 0;
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = 0;
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
  ICR1 = 40000;
  //  OCR1A = 2000;
}

void servoWrite(uint8_t pos) {
  OCR1A = map(pos, 0, 180, 3500, 1200);
}

void stopProgram() {
  temp = 0;
  while (1){
    beep(1, 200);
    lcd.setCursor(0, 3);
    lcd.print(temp ? "  <Task Completed>" : "                    ");
    temp = !temp;
  }
}

void beep(uint8_t times, unsigned long delayMillis){
  for(uint8_t i=0; i<times; i++){
    digitalWrite(buzzerPin, 1);
    delay(delayMillis);
    digitalWrite(buzzerPin, 0);
    delay(delayMillis);
  }
}
