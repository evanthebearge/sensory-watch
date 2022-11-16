/*
   THIS CODE IS FOR THE "SLAVE: WATCH1"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Peripheral/ButtonLED/ButtonLED.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Alpha
  Updated: 11/16/22 - 1:42 PM
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
const int led = D2;
const int motor = D3;
const int button0 = D0;
const int button1 = D1;

// define button states
int oldbutton0state = LOW;
int oldbutton1state = LOW;

// create Bluetooth services
BLEService ledServiceWATCH1("3000");
BLEService motorServiceWATCH1("4000");
BLEService button0ServiceWATCH1("1000");
BLEService button1ServiceWATCH1("2000");

// create characteristics to allow WATCH0 to read, write, and notify
BLEByteCharacteristic ledCharacteristicWATCH1("3001", BLERead | BLEWrite | BLENotify); 
BLEByteCharacteristic motorCharacteristicWATCH1("4001", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button0CharacteristicWATCH1("1001", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button1CharacteristicWATCH1("2001", BLERead | BLEWrite | BLENotify);

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

// set advertised local name and service UUID:
  BLE.setLocalName("WATCH1");
  BLE.setAdvertisedServiceUuid("0000");
  BLE.setAdvertisedService("ledServiceWATCH1");
  BLE.setAdvertisedService("motorServiceWATCH1");
  BLE.setAdvertisedService("button0ServiceWATCH1");
  BLE.setAdvertisedService("button1ServiceWATCH1");

// add the characteristics to the services
  ledServiceWATCH1.addCharacteristic("ledCharacteristicWATCH1");
  motorServiceWATCH1.addCharacteristic("motorCharacteristicWATCH1");
  button0ServiceWATCH1.addCharacteristic("button0CharacteristicWATCH1");
  button1ServiceWATCH1.addCharacteristic("button1CharacteristicWATCH1");

// add services
  BLE.addService(ledServiceWATCH1);
  BLE.addService(motorServiceWATCH1);
  BLE.addService(button0ServiceWATCH1);
  BLE.addService(button1ServiceWATCH1);

// set the initial value for the characeristics:
  ledCharacteristicWATCH1.writeValue((byte)0x00);
  motorCharacteristicWATCH1.writeValue((byte)0x00);
  button0CharacteristicWATCH1.writeValue((byte)0x00);
  button1CharacteristicWATCH1.writeValue((byte)0x00);

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
    }
// while the central is still connected to peripheral:
    while (central.connected) {
      checkForWrites;
      checkForButtonPress;
    }

// when the central disconnects, print it out:
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
}

// recieving section
void checkForWrites() {
// led control
  if (ledCharacteristicWATCH0.written()) {
    if (ledCharacteristicWATCH0.value()) {
      Serial.println("LED on");
      digitalWrite(led, HIGH); // changed from HIGH to LOW
    } else {
      Serial.println(F("LED off"));
      digitalWrite(led, LOW); // changed from LOW to HIGH
    }

// transmitting section
void checkForButtonPress() {

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
  }
