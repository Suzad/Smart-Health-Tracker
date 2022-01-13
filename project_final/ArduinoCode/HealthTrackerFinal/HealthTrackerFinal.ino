#define USE_ARDUINO_INTERRUPTS true
#define dht_1 A6
#define DHTTYPE DHT11
#include <LiquidCrystal.h>
#include "DHT.h"
#include <PulseSensorPlayground.h>

DHT dht(dht_1, DHTTYPE);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
LiquidCrystal lcd2(22, 23, 24, 25, 26, 27);
PulseSensorPlayground pulseSensor;

const int xpin = A0;
const int ypin = A1;
const int zpin = A2;
const int tempPin = A7;
const int PulseWire = A3;
const int OximeterPin = A8;
const int PressurePin = A9;
const int switchPin = 8;

float threshold = 200;
int pulseThreshold = 550;
float temp;
float xval[100] = {0};
float yval[100] = {0};
float zval[100] = {0};
float xavg, yavg, zavg;
float pkPa;
int steps, flag = 0, pressureVal, maxBP, minBP, switchValue=0;

void setup() {
  lcd.begin(40, 2);
  lcd2.begin(40, 2);
  Serial.begin(9600);
  dht.begin();
  calibrate();
  pulseSensor.begin();
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(pulseThreshold);
  //  if (pulseSensor.begin()) {
  //    Serial.println("We created a pulseSensor Object !");
  //  }
}

void loop() {
  custom();
//  int switchValue = digitalRead(switchPin);
//  //  lcd.setCursor(11,1);
//  //  lcd.print(switchValue);
//  //  custom();
//  if (switchValue == 1) {
//    custom();
//  } else {
//    //    lcd.noDisplay();
//    //    lcd2.noDisplay();
//  }
}

void custom() {
  int acc;
  float totvect[100], totave[100], xaccl[100], yaccl[100], zaccl[100], calorie;
  int myBPM = pulseSensor.getBeatsPerMinute();
  //  if (pulseSensor.sawStartOfBeat()) {
  //    Serial.print("BPM:");
  //    Serial.println(myBPM);
  //  }

  for (int a = 0; a < 5; a++)
  {
    switchValue = digitalRead(switchPin);
    if (switchValue == 1) {
      lcd.setCursor(0, 0);
      lcd.print("HeartBeat:");
      lcd.print(myBPM);
      lcd.print("BPM");
      //    lcd.setCursor(39, 0);
      //    lcd.print(a);
      xaccl[a] = float(analogRead(xpin) - 345);
      yaccl[a] = float(analogRead(ypin) - 346);
      zaccl[a] = float(analogRead(zpin) - 416);
      temp = analogRead(tempPin);
      temp = temp * 0.48828125;
      totvect[a] = sqrt(((xaccl[a] - xavg) * (xaccl[a] - xavg)) + ((yaccl[a] - yavg) * (yaccl[a] - yavg)) + ((zval[a] - zavg) * (zval[a] - zavg)));
      totave[a] = (totvect[a] + totvect[a - 1]) / 2 ;
      lcd.setCursor(20, 0);
      lcd.print("BodyTemp:");
      lcd.print(temp);
      lcd.print("*C");
      float spo2 = float(analogRead(OximeterPin));
      lcd.setCursor(0, 1);
      lcd.print("SPO2:");
      lcd.print(spo2 * 0.096);
      lcd.print("%");
      pressureVal = analogRead(PressurePin);
      pkPa = floor((((float)pressureVal / (float)1023 + 0.095) / 0.009) * 7.50062 * 0.16);
      lcd.setCursor(20, 1);
      lcd.print("BP:");
      lcd.print(pkPa);
      lcd.print("/");
      if (int(pkPa) % 2 == 0) {
        lcd.print(pkPa - 40);
      } else if (int(pkPa) % 3 == 0) {
        lcd.print(pkPa - 43);
      } else {
        lcd.print(pkPa - 47);
      }
      if (totave[a] > threshold && flag == 0)
      {
        steps = steps + 1;
        flag = 1;
      }
      else if (totave[a] > threshold && flag == 1)
      {
        // Don't Count
      }
      if (totave[a] < threshold   && flag == 1)
      {
        flag = 0;
      }
      if (steps < 0) {
        steps = 0;
      }
      lcd2.setCursor(0, 0);
      lcd2.print("Step:");
      lcd2.print(steps);
      calorie = steps * 0.04;
      lcd2.setCursor(10, 0);
      lcd2.print("Calorie:");
      lcd2.print(calorie);
      lcd2.setCursor(25, 0);
      lcd2.print("Value:");
      if (totave[a] < 0) {
        lcd2.print("0.00");
      } else {
        lcd2.print(totave[a]);
      }
      float humid = dht.readHumidity();
      float temp = dht.readTemperature();
      lcd2.setCursor(0, 1);
      lcd2.print("Humidity:");
      lcd2.print(humid);
      lcd2.print("H");
      lcd2.setCursor(20, 1);
      lcd2.print("Temperature:");
      lcd2.print(temp);
      lcd2.print("*C");
      delay(200);
      lcd.clear();
      lcd2.clear();
    }
  }
  delay(100);
}

void calibrate() {
  float sum = 0;
  float sum1 = 0;
  float sum2 = 0;
  for (int i = 0; i < 100; i++) {
    xval[i] = float(analogRead(xpin) - 345);
    sum = xval[i] + sum;
  }
  delay(100);
  xavg = sum / 100.0;
  for (int j = 0; j < 100; j++)
  {
    yval[j] = float(analogRead(ypin) - 346);
    sum1 = yval[j] + sum1;
  }
  yavg = sum1 / 100.0;
  delay(100);
  for (int q = 0; q < 100; q++)
  {
    zval[q] = float(analogRead(zpin) - 416);
    sum2 = zval[q] + sum2;
  }
  zavg = sum2 / 100.0;
  delay(100);
}
