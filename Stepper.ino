
void RunStepper() {
  if (stepperClock) {
    myStepper.step(1); // one step at a time
  }
  else {
    myStepper.step(-1); // one step at a time
  }
}
