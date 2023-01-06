/*
   THIS CODE IS FOR THE "MASTER: WATCH0"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Peripheral/CallbackLED/CallbackLED.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Beta
  Updated: 1/5/22 - 10:48 PM
  TO-DO:
  TEST CODE
  CONNECT/DISCONNECT ALERT
  PROPERLY DEFINED FUNCTIONS (WHAT BUTTONS WILL ACTUALLY DO FINAL)
*/

#include <ArduinoBLE.h>

//define pins NOTE: WATCH1 IS DIFFERENT
const int led = D2;
const int motor = D3;
const int button0 = D0;
const int button1 = D1;

// variables for button
int oldbutton0state = LOW;
int oldbutton1state = LOW;

void setup() {
// enable lithium cell battery charging
  pinMode(P0_13, OUTPUT);
    
// enable serial logging    
  Serial.begin(9600);

// set pins to input/output mode
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);

// begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth Low Energy module failed!");

    while (1);
  }
  
  Serial.println("WATCH0");

// start scanning for WATCH1
  BLE.scanForName("WATCH1");
}

void loop() {
  
// check if WATCH0 (peripheral) has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    
// discovered WATCH0 (peripheral), print out address, local name, and advertised services
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

// make sure peripheral is WATCH1
    if (peripheral.localName() != "WATCH1") {
      return;
    }
    
// stop scanning for WATCH1
    // stop scanning
    BLE.stopScan();

// run watch script
    control(peripheral);

// if it disconnects, restart scanning for WATCH1
    BLE.scanForName("WATCH1");
  }
}

void control(BLEDevice peripheral) {

// connect to the WATCH1 (peripheral)
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

// discover WATCH1 (peripheral) attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

// retrieve the characteristics from WATCH1 and verify
  BLECharacteristic ledCharacteristicWATCH0 = peripheral.characteristic("30000001-0000-0000-0000-000000000000"); // 
  BLECharacteristic motorCharacteristicWATCH0 = peripheral.characteristic("40000001-0000-0000-0000-000000000000"); // 
  BLECharacteristic ledCharacteristicWATCH1 = peripheral.characteristic("10000001-0000-0000-0000-000000000000"); // 
  BLECharacteristic motorCharacteristicWATCH1 = peripheral.characteristic("20000001-0000-0000-0000-000000000000"); // 
  if (!ledCharacteristicWATCH0) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristicWATCH0.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }
  
// While the WATCH1 (peripheral) is connected
  while (peripheral.connected()) {

// read the button pins
    int button0state = digitalRead(button0);
    int button1state = digitalRead(button0);

    if (oldbutton0state != button0state) {
      
// button0 changed
      oldbutton0state = button0state;

      if (button0state) {
        Serial.println("button0 pressed");

// button0 is pressed, write 0x01 to turn the LED on
        button0CharacteristicWATCH1.writeValue((byte)0x01);
      } else {
        Serial.println("button0 released");

// button0 is released, write 0x00 to turn the LED off
        button0CharacteristicWATCH1.writeValue((byte)0x00);
      }
    }
  }

  Serial.println("WATCH1 disconnected");
}
