void setup() {
  // pinMode P0_13, OUTPUT); // Initiate battery charging
  Serial.begin(9600);
  setupBlueToothConnection();
  delay(1000);
  Serial.flush();
  blueToothSerial.flush();
  pinMode 
}
void loop() {
  // digitalWrite(P0_13, LOW); // 100mA battery charging current
}
