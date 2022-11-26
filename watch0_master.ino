/*
   THIS CODE IS FOR THE "MASTER: WATCH0"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Central/LedControl/LedControl.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Beta
  Updated: 11/25/22 - 9:21 PM
  TO-DO:
  TEST CODE
  CONNECT/DISCONNECT ALERT
  PROPERLY DEFINED FUNCTIONS (WHAT BUTTONS WILL ACTUALLY DO FINAL)
*/
#include <ArduinoBLE.h>

// define pins
const int led = D2;
const int motor = D3;
const int button0 = D0;
const int button1 = D1;

// define button states
int oldbutton0state = LOW;
int oldbutton1state = LOW;

void setup() {

// enable lithium cell battery charging
  pinMode(P0_13, OUTPUT);
    
  Serial.begin(9600);
  while (!Serial);

// set pins to input/output mode
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);

// initialize the Bluetooth
  BLE.begin();

  Serial.println("WATCH0");

// start scanning for WATCH1
  BLE.scanForUuid("00000000-0000-0000-0000-000000000000");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
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
    BLE.stopScan();

// do watch script
    control(peripheral);

// connect to the peripheral WATCH1
    BLE.scanForUuid("00000000-0000-0000-0000-000000000000"); 
  }
}  

void control(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

// discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

// retrieve the characteristics for WATCH1 from WATCH1
  BLECharacteristic ledCharacteristicWATCH1 = peripheral.characteristic("30000001-0000-0000-0000-000000000000");
  BLECharacteristic motorCharacteristicWATCH1 = peripheral.characteristic("40000001-0000-0000-0000-000000000000");
  BLECharacteristic button0CharacteristicWATCH1 = peripheral.characteristic("10000001-0000-0000-0000-000000000000");
  BLECharacteristic button1CharacteristicWATCH1 = peripheral.characteristic("20000001-0000-0000-0000-000000000000");
// retrieve the characteristics for WATCH0 from WATCH1
  BLECharacteristic ledCharacteristicWATCH0 = peripheral.characteristic("30010001-0000-0000-0000-000000000000");
  BLECharacteristic motorCharacteristicWATCH0 = peripheral.characteristic("40010001-0000-0000-0000-000000000000");
  BLECharacteristic button0CharacteristicWATCH0 = peripheral.characteristic("10010001-0000-0000-0000-000000000000");
  BLECharacteristic button1CharacteristicWATCH0 = peripheral.characteristic("20010001-0000-0000-0000-000000000000");

  if (!ledCharacteristicWATCH1) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristicWATCH1.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }

// set the initial value for the characeristics WATCH0:
  ledCharacteristicWATCH0.writeValue((byte)0x00);
  motorCharacteristicWATCH0.writeValue((byte)0x00);
  button0CharacteristicWATCH0.writeValue((byte)0x00);
  button1CharacteristicWATCH0.writeValue((byte)0x00);
// set the initial value for the characeristics WATCH1:
  ledCharacteristicWATCH1.writeValue((byte)0x00);
  motorCharacteristicWATCH1.writeValue((byte)0x00);
  button0CharacteristicWATCH1.writeValue((byte)0x00);
  button1CharacteristicWATCH1.writeValue((byte)0x00);

// while WATCH1 is connected
  while (peripheral.connected()) {

// recieve from WATCH1
  if (ledCharacteristicWATCH1.written()) {
    if (ledCharacteristicWATCH1.value()) {
      Serial.println("LED on");
      digitalWrite(led, HIGH); // changed from HIGH to LOW
    } else {
      Serial.println("LED off");
      digitalWrite(led, LOW); // changed from LOW to HIGH
    }
    if (motorCharacteristicWATCH1.written()) {
      if (motorCharacteristicWATCH1.value()) {
        Serial.println("Motor on");
        digitalWrite(motor, HIGH); // changed from HIGH to LOW
      } else {
        Serial.println("Motor off");
        digitalWrite(motor, LOW); // changed from LOW to HIGH
      }
    }
  }

// send to WATCH1

// read the button pins
    int button0state = digitalRead(button0);
    int button1state = digitalRead(button1);

// button 0 section
    if (oldbutton0state != button0state) {
// button0 changed
      oldbutton0state = button0state;

      if (button0state) {
        Serial.println("button0 pressed");

// button0 is pressed, write 0x01 to bluetooth to turn the motor on
        motorCharacteristicWATCH1.writeValue((byte)0x01);
      } else {
        Serial.println("button0 released");

// button0 is released, write 0x00 to turn the motor off
        motorCharacteristicWATCH1.writeValue((byte)0x00);
      }
    } 
    
// button1 section      
    if (oldbutton1state != button1state) {
      // button1 changed
      oldbutton1state = button1state;

      if (button1state) {
        Serial.println("button1 pressed");

// button1 is pressed, write 0x01 to bluetooth to turn the LED on
        ledCharacteristicWATCH1.writeValue((byte) 0x01);
      } else {
        Serial.println("button1 released");

// button1 is released, write 0x00 to bluetooth turn the LED off
        ledCharacteristicWATCH1.writeValue((byte) 0x00);
      }
    }
  }
// when the peripheral disconnects, print it out:
  Serial.println("WATCH1 disconnected");
}
