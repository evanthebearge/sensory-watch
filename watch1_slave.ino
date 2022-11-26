/*
   THIS CODE IS FOR THE "SLAVE: WATCH1"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Peripheral/ButtonLED/ButtonLED.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Beta
  Updated: 11/25/22 - 22:10 PM
  TO-DO:
  TEST CODE
  LOW BATTERY ALERT
  CONNECT/DISCONNECT ALERT
  PROPERLY DEFINED FUNCTIONS (WHAT BUTTONS WILL ACTUALLY DO FINAL)
*/

#include <ArduinoBLE.h>

// create Bluetooth services for WATCH1
BLEService ledServiceWATCH1("30000000-0000-0000-0000-000000000000");
BLEService motorServiceWATCH1("40000000-0000-0000-0000-000000000000");
BLEService button0ServiceWATCH1("10000000-0000-0000-0000-000000000000");
BLEService button1ServiceWATCH1("20000000-0000-0000-0000-000000000000");
// create Bluetooth services for WATCH0
BLEService ledServiceWATCH0("30010000-0000-0000-0000-000000000000");
BLEService motorServiceWATCH0("40010000-0000-0000-0000-000000000000");
BLEService button0ServiceWATCH0("10010000-0000-0000-0000-000000000000");
BLEService button1ServiceWATCH0("20010000-0000-0000-0000-000000000000");

// create characteristics to allow WATCH0 to read, write, and notify
BLEByteCharacteristic ledCharacteristicWATCH1("30000001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic motorCharacteristicWATCH1("40000001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button0CharacteristicWATCH1("10000001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button1CharacteristicWATCH1("20000001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);
// create characteristics to allow WATCH1 to read, write, and notify
BLEByteCharacteristic ledCharacteristicWATCH0("30010001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic motorCharacteristicWATCH0("40010001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button0CharacteristicWATCH0("10010001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button1CharacteristicWATCH0("20010001-0000-0000-0000-000000000000", BLERead | BLEWrite | BLENotify);

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
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth module failed!");

    while (1);
  }

// set local name and advertised service UUID:
  BLE.setLocalName("WATCH1");
  BLE.setAdvertisedServiceUuid("00000000-0000-0000-0000-000000000000");

// set advertised services WATCH1
  BLE.setAdvertisedService(ledServiceWATCH1);
  BLE.setAdvertisedService(motorServiceWATCH1);
  BLE.setAdvertisedService(button0ServiceWATCH1);
  BLE.setAdvertisedService(button1ServiceWATCH1);
// set advertised services WATCH0
  BLE.setAdvertisedService(ledServiceWATCH0);
  BLE.setAdvertisedService(motorServiceWATCH0);
  BLE.setAdvertisedService(button0ServiceWATCH0);
  BLE.setAdvertisedService(button1ServiceWATCH0);

// add the characteristics to the services WATCH1
  ledServiceWATCH1.addCharacteristic(ledCharacteristicWATCH1);
  motorServiceWATCH1.addCharacteristic(motorCharacteristicWATCH1);
  button0ServiceWATCH1.addCharacteristic(button0CharacteristicWATCH1);
  button1ServiceWATCH1.addCharacteristic(button1CharacteristicWATCH1);
// add the characteristics to the services WATCH0
  ledServiceWATCH0.addCharacteristic(ledCharacteristicWATCH0);
  motorServiceWATCH0.addCharacteristic(motorCharacteristicWATCH0);
  button0ServiceWATCH0.addCharacteristic(button0CharacteristicWATCH0);
  button1ServiceWATCH0.addCharacteristic(button1CharacteristicWATCH0);

// add services WATCH1
  BLE.addService(ledServiceWATCH1);
  BLE.addService(motorServiceWATCH1);
  BLE.addService(button0ServiceWATCH1);
  BLE.addService(button1ServiceWATCH1);
// add services WATCH0
  BLE.addService(ledServiceWATCH0);
  BLE.addService(motorServiceWATCH0);
  BLE.addService(button0ServiceWATCH0);
  BLE.addService(button1ServiceWATCH0);

// set the initial value for the characeristics WATCH1
  ledCharacteristicWATCH1.writeValue((byte)0x00);
  motorCharacteristicWATCH1.writeValue((byte)0x00);
  button0CharacteristicWATCH1.writeValue((byte)0x00);
  button1CharacteristicWATCH1.writeValue((byte)0x00);
// set the initial value for the characeristics WATCH0
  ledCharacteristicWATCH0.writeValue((byte)0x00);
  motorCharacteristicWATCH0.writeValue((byte)0x00);
  button0CharacteristicWATCH0.writeValue((byte)0x00);
  button1CharacteristicWATCH0.writeValue((byte)0x00);

// start advertising
  BLE.advertise();

  Serial.println("Bluetooth active, waiting for WATCH0");
}

void loop() {

// battery high charging current (100mA)
  digitalWrite(P0_13, LOW);

// listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

// if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
// print the central's MAC address:
    Serial.println(central.address());
    
// while the central is still connected to peripheral:
  while (central.connected()) {

// recieve from WATCH0
  if (ledCharacteristicWATCH0.written()) {
    if (ledCharacteristicWATCH0.value()) {
      Serial.println("LED on");
      digitalWrite(led, HIGH); // changed from HIGH to LOW
    } else {
      Serial.println("LED off");
      digitalWrite(led, LOW); // changed from LOW to HIGH
    }
    if (motorCharacteristicWATCH0.written()) {
      if (motorCharacteristicWATCH0.value()) {
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

// button0 section
        if (oldbutton0state != button0state) {
// button0 changed
          Serial.println("button0 pressed");

// button0 is pressed, write 0x01 to bluetooth to turn the motor on
          motorCharacteristicWATCH0.writeValue((byte) 0x01);
        } else {
          Serial.println("button0 released");

// button0 is released, write 0x00 to bluetooth to turn the motor off
          motorCharacteristicWATCH0.writeValue((byte) 0x00);
        }

// button1 section
        if (oldbutton1state != button1state) {
// button1 change
          Serial.println("button1 pressed");
          // button1 is pressed, write 0x01 to bluetooth to turn the LED on
          ledCharacteristicWATCH0.writeValue((byte) 0x01);
        } else {
          Serial.println("button1 released");
// button1 is released, write 0x00 to bluetooth turn the LED off
          ledCharacteristicWATCH0.writeValue((byte) 0x00);
        }
  }
// when the central disconnects, print it out:
    Serial.print("WATCH0 disconnected");
    Serial.println(central.address());
  }      
}
