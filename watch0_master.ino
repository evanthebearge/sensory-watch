/*
   THIS CODE IS FOR THE "MASTER: WATCH0"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Central/LedControl/LedControl.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Alpha
  Updated: 11/3/22 - 2:27 PM
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
BLEService ledService("3100");
BLEService motorService("4100");
BLEService button0Service("1100");
BLEService button1Service("2100");

// create characteristics to allow watch0 to read, write, and notify
BLEByteCharacteristic ledCharacteristic("3101", BLERead | BLEWrite | BLENotify); 
BLEByteCharacteristic motorCharacteristic("4101", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button0Characteristic("1101", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button1Characteristic("2101", BLERead | BLEWrite | BLENotify);

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
  
  Serial.println("watch0");

// start scanning for watch1
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

// make sure peripheral is watch1
    if (peripheral.localName() != "watch1") {
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

// retrieve the characteristics from watch1
  BLECharacteristic ledCharacteristic = peripheral.characteristic("ledCharacteristic");
  BLECharacteristic motorCharacteristic = peripheral.characteristic("motorCharacteristic")
  BLECharacteristic button0 = peripheral.characteristic("button0Characteristic")
  BLECharacteristic button1 = peripheral.characteristic("button1Characteristic")

// check for other characteristics from watch1 ADD THEM
  if (!ledCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristic.canWrite()) {
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
      ledCharacteristic.writeValue((byte) 0x01);
    } else {
      Serial.println("button0 released");
      
// button0 is released, write 0x00 to bluetooth to turn the LED off
      ledCharacteristic.writeValue((byte) 0x00);
    }
    
    if (oldbutton1state != button1state) {
// button1 change
      Serial.println("button1 pressed");
// button1 is pressed, write 0x01 to bluetooth to turn the motor on
      motrCharacteristic.writeValue((byte) 0x01);
    } else {
      Serial.println("button1 released");
// button1 is released, write 0x00 to bluetooth turn the motor off
      motorCharacteristic.writeValue((byte) 0x00);
    }
   }
  }
Serial.println("watch1 disconnected");
}
