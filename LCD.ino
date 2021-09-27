void LCDRefresh() {
  dt = clock.getDateTime();
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(4, 0);
  lcd.print(dt.hour);
  lcd.print(":");
  lcd.print(dt.minute);
  lcd.print(":");
  lcd.print(dt.second);
}
