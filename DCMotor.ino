
void RunDCMotor(boolean clockwise, uint8_t motSpeed) {
  if (clockwise) {
    digitalWrite(DCMotorDirA, HIGH); 
    digitalWrite(DCMotorDirB, LOW);
  }
  else {
    digitalWrite(DCMotorDirA, LOW); 
    digitalWrite(DCMotorDirB, HIGH);
  }
  digitalWrite(DCMotorEna, motSpeed); 
}
void StopDCMotor() {
  digitalWrite(DCMotorDirA, LOW); 
  digitalWrite(DCMotorDirB, LOW);
  digitalWrite(DCMotorEna, 0); 
  DCMotorRunning=false;
}
