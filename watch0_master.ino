/*
   THIS CODE IS FOR THE "MASTER: WATCH0"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Central/LedControl/LedControl.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Alpha
  Updated: 11/16/22 - 10:32 AM
  TO-DO:
  CONTROLLING CODE FOR LED,MOTOR
  CONTROLLING CODE FOR BUTTON 0,1
  DEFINE INPUT VARIABLE (0,1,2,4,8)
  MAKE BI-DIRECTIONAL COMMUNICATION SLAVE + MASTER
  LOW BATTERY ALERT
  CONNECT/DISCONNECT ALERT
  PAIR ALERT
*/

#include <ArduinoBLE.h>

// define pins
const int led = D3;
const int motor = D4;
const int button0 = D0;
const int button1 = D1;

// define button states
int oldbutton0state = LOW;
int oldbutton1state = LOW;

// create Bluetooth services
BLEService ledServicewatch0("3100");
BLEService motorServicewatch0("4100");
BLEService button0Servicewatch0("1100");
BLEService button1Servicewatch0("2100");

// create characteristics to allow WATCH1 to read, write, and notify
BLEByteCharacteristic ledCharacteristicwatch1("3101", BLERead | BLEWrite | BLENotify); 
BLEByteCharacteristic motorCharacteristicwatch1("4101", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button0Characteristicwatch1("1101", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button1Characteristicwatch1("2101", BLERead | BLEWrite | BLENotify);

void setup() {
  
// enable battery charging
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
  BLE.scanForUuid("0000"); 
}

void loop() {

// battery high charging current (100mA)
  digitalWrite(P0_13, LOW);

// check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    
// discovered watch1, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

// make sure peripheral is WATCH1
    if (peripheral.localName() != "WATCH!") {
      return;
    }

// stop scanning
    BLE.stopScan();

    controlwatch1(peripheral);

// peripheral watch1 disconnected, start scanning again
    BLE.scanForUuid("0000");
  }
}

void controlwatch1(BLEDevice peripheral) {

// connect to the peripheral watch1
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
}

// retrieve the characteristics from WATCH1
  BLECharacteristic ledCharacteristicwatch1 = peripheral.characteristic("1-ledCharacteristic");
  BLECharacteristic motorCharacteristicwatch1 = peripheral.characteristic("1-motorCharacteristic")
  BLECharacteristic button0watch1 = peripheral.characteristic("1-button0Characteristic")
  BLECharacteristic button1watch1 = peripheral.characteristic("1-button1Characteristic")

// check for other characteristics from watch1 ADD THEM
  if (!ledwatch1Characteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledwatch1Characteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
// while the peripheral is connected

// read the button pins
    int button0state = digitalRead(button0);
    int button1state = digitalRead(button1);

    if (oldbutton0state != button0state) {
// button0 changed
      Serial.println("button0 pressed");
      
// button0 is pressed, write 0x01 to bluetooth to turn the LED on
      ledwatch1Characteristic.writeValue((byte) 0x01);
    } else {
      Serial.println("button0 released");
      
// button0 is released, write 0x00 to bluetooth to turn the LED off
      ledwatch1Characteristic.writeValue((byte) 0x00);
    }
    
    if (oldbutton1state != button1state) {
// button1 change
      Serial.println("button1 pressed");
// button1 is pressed, write 0x01 to bluetooth to turn the motor on
      motorwatch1Characteristic.writeValue((byte) 0x01);
    } else {
      Serial.println("button1 released");
// button1 is released, write 0x00 to bluetooth turn the motor off
      motorwatch1Characteristic.writeValue((byte) 0x00);
    }
   }
Serial.println("WATCH1 disconnected");
}
