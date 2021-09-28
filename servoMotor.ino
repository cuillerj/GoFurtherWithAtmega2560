void RunServoMotor() {
  servoMotorClock = !servoMotorClock;  // change rotation sense
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
  myservo.write(90); // center servo position 
  servoRunning = false;
}
