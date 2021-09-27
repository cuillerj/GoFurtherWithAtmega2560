/*
  Written by Jean Cuiller
  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*/
/*

*/
#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Stepper.h>
#include <Servo.h>
/*
   define GPIOs
*/
#define buzzer 7  // PMW
#define DCMotorEna 6  // PMW
#define ballSwitch 18 // interrupt 5
// RTC clock Pins
#define rtcSQWPin 19 // interrupt 4
#define rtcSDA 20 //  I2C SDA
#define rtcSCL 21 //  I2C SCL
/*
   LCD Pins
*/
#define lcdRS 22
#define lcdE 23
#define lcdDB4 24
#define lcdDB5 25
#define lcdDB6 26
#define lcdDB7 27
/*
   DC motor Pins
*/
#define DCMotorDirA 28
#define DCMotorDirB 29
/*
   stepper Pins
*/
#define stepperIn1 31
#define stepperIn2 32
#define stepperIn3 33
#define stepperIn4 34

#define servoMotor 35  // servo motor Pin
/*
   push button Pins
*/
#define switch1 36
#define switch2 37
#define switch3 38


/*
   variable that are modified inside interrupt functions has to be defined as volatile
*/
volatile boolean sqwinterrupt = false;
volatile boolean ballinterrupt = false;

/*
   define LCD object
*/
LiquidCrystal lcd(lcdRS, lcdE, lcdDB4, lcdDB5, lcdDB6, lcdDB7);

/*
   define clock object
*/
DS3231 clock;
RTCDateTime dt;

/*
   define stepper object
*/
#define stepsPerRevolution  2048  // change this to fit the number of steps per revolution
const int rpm = 15;         // Adjustable range of 28BYJ-48 stepper is 0~17 rpm
unsigned int stepsCount = 0;
Stepper myStepper(stepsPerRevolution, stepperIn1, stepperIn3, stepperIn2, stepperIn4);

/*
   define servo motor object
*/
Servo myservo;

/*
   variables used to determine motors running status
*/
boolean DCMotorRunning = false;
boolean stepperRunning = false;
boolean servoRunning = false;

/*
   boolean used to determine clockwise or not rotation
*/
boolean dcMotorClock = true;
boolean stepperClock = true;
boolean servoMotorClock = true;

boolean buzzerOn = true; // determine buzzer status


/*
   variables used to store millis() values
*/
unsigned long switch1Timer;
unsigned long switch2Timer;
unsigned long switch3Timer;
unsigned long ballTimer;
unsigned long buzzTimer;
unsigned long servoTimer;


void setup()
{
  Serial.begin(38400);
  pinMode(rtcSQWPin, INPUT);
  pinMode(ballSwitch, INPUT_PULLUP);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(DCMotorEna, OUTPUT);
  pinMode(DCMotorDirA, OUTPUT);
  pinMode(DCMotorDirA, OUTPUT);
  Serial.println("Initialize RTC module");
  clock.begin();
  clock.setDateTime(__DATE__, __TIME__);
  /*
     enable RTC SQW interrut at 1Hz
  */
  Serial.println("write i2c");
  Wire.beginTransmission(0x68); // transmit to device 68 RTC address
  Wire.write(0x07);             // DS control register to modify 
  /* control register
   * bit 4 used to enable output 0-disable 1-enable
   * bit 1 & 0 used to select interrupt cycle (00-1Hz, 01-4096Hz, 10- 81292Hz, 11-32768Hz)
   */
  Wire.write(0b00010000);  // set bit 4 SQWE  to 1 to enable oscillator output
  Wire.endTransmission();    // stop transmitting

  lcd.begin(16, 2);  // 16 char x 2 lines screen
 
  attachInterrupt(digitalPinToInterrupt(rtcSQWPin), RtcInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(ballSwitch), BallInterrupt, RISING);
  myStepper.setSpeed(rpm);
  myservo.attach(servoMotor);
  myservo.write(90); // center servo position 
  PrintTime();
}

void loop() {
  if (sqwinterrupt) {
    sqwinterrupt = false;
    LCDRefresh();
  }
  if (ballinterrupt && millis() - ballTimer > 500) {
    ballTimer = millis();
    buzzTimer = millis();
    ballinterrupt = false;
    buzzerOn = true;
    Serial.print("ball:");
    Serial.println(digitalRead(ballSwitch));
    if (digitalRead(ballSwitch)) {
      tone(buzzer, 523);
    }
    else {
      tone(buzzer, 880);
      StopDCMotor();
      stepperRunning = false;
      StopservoMotor();
    }
    attachInterrupt(digitalPinToInterrupt(ballSwitch), BallInterrupt, RISING);
  }
  if (buzzerOn && millis() - buzzTimer > 1000) {
    noTone(buzzer);
    buzzerOn = false;
  }
  if (!digitalRead(switch1) && millis() - switch1Timer > 500) {
    Serial.println("switch1");
    switch1Timer = millis();
    DCMotorRunning = !DCMotorRunning;
    if (DCMotorRunning) {
      RunDCMotor(dcMotorClock, 150);
      dcMotorClock = !dcMotorClock;
    }
    else {
      StopDCMotor();
    }
  }
  if (!digitalRead(switch2) && millis() - switch2Timer > 500) {
    Serial.println("switch2");
    switch2Timer = millis();
    stepperRunning = !stepperRunning;
  }
  if (!digitalRead(switch3) && millis() - switch3Timer > 500) {
    Serial.println("switch3");
    switch3Timer = millis();
    servoRunning = !servoRunning;
    if (servoRunning) {
      RunServoMotor();
    }
    else {
      StopservoMotor();
    }
  }
  if (stepperRunning) {
    RunStepper();
    stepsCount++;
    if (stepsCount > stepsPerRevolution) {
      stepperClock = !stepperClock;
      stepsCount = 0;
    }
  }
  if (servoRunning && millis() - servoTimer > 1000) {
    Serial.print(".");
    RunServoMotor();
  }
  if (!servoRunning && millis() - servoTimer > 500) {
    myservo.detach();
  }
}

void RtcInterrupt() {
  sqwinterrupt = true;
}
void BallInterrupt() {
  detachInterrupt(digitalPinToInterrupt(ballSwitch));
  ballinterrupt = true;
}
