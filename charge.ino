void setup() {
  pinMode P0_13, OUTPUT); // Initiate battery charging
}
void loop() {
  digitalWrite(P0_13, LOW); // 100mA battery charging current
}
