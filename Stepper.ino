
void RunStepper() {
  if (stepperClock) {
    myStepper.step(1);
  }
  else {
    myStepper.step(-1);
  }
}
