void RunServoMotor() {
  servoMotorClock = !servoMotorClock;
  int degres = 160;
  if (!servoMotorClock) {
    degres = 20;
  }
  servoTimer = millis();
  myservo.attach(servoMotor);
  myservo.write(degres);

}
void StopservoMotor() {
  servoTimer = millis();
  myservo.write(90);
  servoRunning = false;
}
