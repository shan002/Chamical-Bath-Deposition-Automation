#include <DS18B20.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 20, 4);

OneWire  ds(6);  // on pin 10 (a 4.7K resistor is necessary)
byte addressIn[8];
void configure_sensor(void);
byte type_s;

#include "src/Rotary_Shan/Rotary_Shan.h"
Rotary_Shan rotary(3, 4, 2);

#include "src/Simple_Timer/timer.h"
void updateTime(unsigned long passedTime);
Timer timer(1000, updateTime);

#define outSensorPin 11
#define inSensorPin 6
#define buzzerPin 10

DS18B20 dsIn(inSensorPin);
DS18B20 dsOut(outSensorPin);

//const byte leftArrow[] = { B00001, B00011, B00111, B01111, B01111, B00111, B00011, B00001};
//const byte rightArrow[] = { B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000};

//uint8_t addressOut[] = {0x28, 0x93, 0xD6, 0x03, 0x00, 0x00, 0x00, 0xBC};
//uint8_t addressIn[] = {0x28, 0xF2, 0x4A, 0x02, 0x00, 0x00, 0x00, 0x51};

const float tempMax = 100.0;
float tempIn, tempOut, preTempIn, preTempOut;
float setTempIn, preSetTempIn;
float tempIndiff;
uint16_t runTime, preRunTime;
uint8_t stirrerOn = 0;
uint8_t temp;
bool isTimerOn = 0;

unsigned long passedTime = 0;
unsigned long timeMillis;
unsigned long tempIntic, tempOfftic;
bool crossedTarget;

float restrictValue(float value, float minValue = 0.0, float maxValue = tempMax);

void setup() {
  Serial.begin(9600);
  configure_sensor();
  pinMode(buzzerPin, OUTPUT);
  lcd.begin();
  lcd.backlight();
//  lcd.createChar(0, leftArrow);
//  lcd.createChar(1, rightArrow);
  servoInit();
  servoWrite(100);

  setTempIn = eepromReadInt(0) / 10.0;
  runTime = eepromReadInt(4);

  preSetTempIn = setTempIn;
  preRunTime = runTime;

  rotary.attachRotationInterrupt(rotationISR);
  rotary.attachSwitchInterrupt(switchISR);

  lcd.setCursor(4, 0);
  lcd.print("Temperature");
  lcd.setCursor(4, 1);
  lcd.print("Controller");
  delay(2000);

  lcd.clear();
  if (!dsIn.select(addressIn)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor-In Error!");
  }
  while (!dsIn.select(addressIn));
  lcd.setCursor(0, 0);
  lcd.print("Sensor-In Connected ");

//  if (!dsOut.select(addressOut)) {
//    lcd.setCursor(0, 1);
//    lcd.print("Sensor-Out Error!");
//  }
//  while (!dsOut.select(addressOut));
//  lcd.setCursor(0, 1);
//  lcd.print("Sensor-Out Connected");
  delay(2000);
  getRunParams();
  showRunPage();
  tempIntic = millis();
  tempOfftic = millis();

  if (dsIn.getTempC() < setTempIn) {
    servoPress();
    stirrerOn = 1;
    tempIntic = millis();
    tempIndiff = setTempIn - dsIn.getTempC();
    crossedTarget = 0;
  }
}

void loop() {
  tempIn = dsIn.getTempC();
  tempOut = dsOut.getTempC();
  if (tempIn != preTempIn) {
    updateDisPreTempIn(tempIn);
    preTempIn = tempIn;
  }
  if (tempOut != preTempOut) {
    updateDisPreTempOut(tempOut);
    preTempOut = tempOut;
  }

  if(tempIn >= 125){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp Out of range");
    while(dsIn.getTempC() >= 125){
      lcd.setCursor(5, 3);
      lcd.print("    ");
      lcd.setCursor(5, 3);
      lcd.print(dsIn.getTempC());
    }
  }

  if(tempIn < 0){
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Sensor Error");
    while(1);
  }

  if (tempIn >= setTempIn && !isTimerOn) {
    beep(3, 100);
    timer.setTimerOn();
    isTimerOn = 1;
  }

  if (!crossedTarget) {
    if (tempIn < (setTempIn - 5) && !stirrerOn) {
      if ((millis() - tempOfftic) > 30000) {
        servoPress();
        stirrerOn = 1;
        tempIntic = millis();
        tempIndiff = setTempIn - tempIn;
      }
    } else if (tempIn > (setTempIn - 5) && stirrerOn) {
      servoPress();
      stirrerOn = 0;
    }
    if (tempIn > setTempIn) {
      crossedTarget = 1;
    }
    if ((millis() - tempIntic) > tempIndiff * 2000 && stirrerOn) {
      servoPress();
      stirrerOn = 0;
      tempOfftic = millis();
    }
  } else {
    if (tempIn < (setTempIn - 1) && !stirrerOn) {
      servoPress();
      stirrerOn = 1;
    } else if (tempIn >= setTempIn && stirrerOn) {
      servoPress();
      stirrerOn = 0;
    }
  }

  timer.run();
}
