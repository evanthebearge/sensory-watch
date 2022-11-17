/*
   THIS CODE IS FOR THE "MASTER: WATCH0"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Central/LedControl/LedControl.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Beta
  Updated: 11/16/22 - 12:40 PM
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

// create Bluetooth services
BLEService ledServiceWATCH0("3100");
BLEService motorServiceWATCH0("4100");
BLEService button0ServiceWATCH0("1100");
BLEService button1ServiceWATCH0("2100");

// create characteristics to allow WATCH1 to read, write, and notify
BLEByteCharacteristic ledCharacteristicWATCH0("3101", BLERead | BLEWrite | BLENotify); 
BLEByteCharacteristic motorCharacteristicWATCH0("4101", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button0CharacteristicWATCH0("1101", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button1CharacteristicWATCH0("2101", BLERead | BLEWrite | BLENotify);
//
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
  BLE.scanForUuid("0000"); 
}

void loop() {

// battery high charging current (100mA)
  digitalWrite(P0_13, LOW);

// check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    
// discovered WATCH1, print out address, local name, and advertised service
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

/*
POSSIBLY REMOVE?
// set advertised services
  BLE.setAdvertisedService("ledServiceWATCH0");
  BLE.setAdvertisedService("motorServiceWATCH0");
  BLE.setAdvertisedService("button0ServiceWATCH0");
  BLE.setAdvertisedService("button1ServiceWATCH0");
    
// add the characteristics to the services
  ledServiceWATCH0.addCharacteristic("ledCharacteristicWATCH0");
  motorServiceWATCH0.addCharacteristic("motorCharacteristicWATCH0");
  button0ServiceWATCH0.addCharacteristic("button0CharacteristicWATCH0");
  button1ServiceWATCH0.addCharacteristic("button1CharacteristicWATCH0");

// add services
  BLE.addService(ledServiceWATCH0);
  BLE.addService(motorServiceWATCH0);
  BLE.addService(button0ServiceWATCH0);
  BLE.addService(button1ServiceWATCH0);
*/

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
  
// stop scanning
    BLE.stopScan();

    control(peripheral);

// peripheral WATCH1 disconnected, start scanning again
    BLE.scanForUuid("0000");
  }
}

void control(BLEDevice peripheral) {

// connect to the peripheral WATCH1
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

// retrieve the characteristics for WATCH1 from WATCH1
  BLECharacteristic ledCharacteristicWATCH1 = peripheral.characteristic("ledCharacteristicWATCH1");
  BLECharacteristic motorCharacteristicWATCH1 = peripheral.characteristic("motorCharacteristicWATCH1")
  BLECharacteristic button0CharacteristicWATCH1 = peripheral.characteristic("button0CharacteristicWATCH1")
  BLECharacteristic button1CharacteristicWATCH1 = peripheral.characteristic("button1CharacteristicWATCH1")

// retrieve the characteristics for WATCH0 from WATCH1
  BLECharacteristic ledCharacteristicWATCH0 = peripheral.characteristic("ledCharacteristicWATCH0");
  BLECharacteristic motorCharacteristicWATCH0 = peripheral.characteristic("motorCharacteristicWATCH0")
  BLECharacteristic button0CharacteristicWATCH0 = peripheral.characteristic("button0CharacteristicWATCH0")
  BLECharacteristic button1CharacteristicWATCH0 = peripheral.characteristic("button1CharacteristicWATCH0")

// check for other characteristics from WATCH1 !!ADD THEM!!
  if (!ledCharacteristicWATCH1) {
    Serial.println("Peripheral WATCH1 does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristicWATCH1.canWrite()) {
    Serial.println("Peripheral WATCH1 does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }
  
// while the peripheral is connected
  while (peripheral.connected()) {
      WATCH1();
      WATCH0();
    }
    
// recieve from WATCH1
void WATCH1() {
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
    
// send to WATCH0
void WATCH0() {

// read the button pins
    int button0state = digitalRead(button0);
    int button1state = digitalRead(button1);

// button0 section
    if (oldbutton0state != button0state) {
      
// button0 changed
      Serial.println("button0 pressed");
      
// button0 is pressed, write 0x01 to bluetooth to turn the LED on
      ledCharacteristicWATCH1.writeValue((byte) 0x01);
    } else {
      Serial.println("button0 released");
      
// button0 is released, write 0x00 to bluetooth to turn the LED off
      ledCharacteristicWATCH1.writeValue((byte) 0x00);
    }

// button1 section   
    if (oldbutton1state != button1state) {
      
// button1 changed
      Serial.println("button1 pressed");
      
// button1 is pressed, write 0x01 to bluetooth to turn the motor on
      motorCharacteristicWATCH1.writeValue((byte) 0x01);
    } else {
      Serial.println("button1 released");
      
// button1 is released, write 0x00 to bluetooth turn the motor off
      motorCharacteristicWATCH1.writeValue((byte) 0x00);
    }
  }
Serial.println("WATCH1 disconnected");
}
