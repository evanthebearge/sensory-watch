/*
   THIS CODE IS FOR THE "SLAVE: WATCH1"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Peripheral/ButtonLED/ButtonLED.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Alpha
  Updated: 11/4/22 - 8:31 AM
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
BLEService ledServicewatch1("3000");
BLEService motorServicewatch1("4000");
BLEService button0Servicewatch1("1000");
BLEService button1Servicewatch1("2000");

// create characteristics to allow watch0 to read, write, and notify
BLEByteCharacteristic ledCharacteristicwatch1("3001", BLERead | BLEWrite | BLENotify); 
BLEByteCharacteristic motorCharacteristicwatch1("4001", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button0Characteristicwatch1("1001", BLERead | BLEWrite | BLENotify);
BLEByteCharacteristic button1Characteristicwatch1("2001", BLERead | BLEWrite | BLENotify);

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
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth module failed!");

    while (1);
  }

// set advertised local name and service UUID:
  BLE.setLocalName("WATCH1");
  BLE.setAdvertisedServiceUuid("0000");
  BLE.setAdvertisedService(ledService);
  BLE.setAdvertisedService(motorService);
  BLE.setAdvertisedService(button0Service);
  BLE.setAdvertisedService(button1Service);

 // add the characteristics to the services
  ledService.addCharacteristic(ledCharacteristic);
  motorService.addCharacteristic(motorCharacteristic);
  button0Service.addCharacteristic(button0Characteristic);
  button1Service.addCharacteristic(button1Characteristic);

 // add services
  BLE.addService(ledServicewatch1);
  BLE.addService(motorServicewatch1);
  BLE.addService(button0Servicewatch1);
  BLE.addService(button1Servicewatch1);

// set the initial value for the characeristics:
  ledCharacteristic.writeValue((byte)0x00);
  motorCharacteristic.writeValue((byte)0x00);
  button0Characteristic.writeValue((byte)0x00);
  button1Characteristic.writeValue((byte)0x00);

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
    while (central.connected( {
      checkForWrites;
      checkForButtonPress;
    }

// when the central disconnects, print it out:
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void checkForWrites() {
// led control
  if (ledCharacteristicwatch0.written()) {
    if (ledCharacteristicwatch0.value()) {
      Serial.println("LED on");
      digitalWrite(led, HIGH); // changed from HIGH to LOW
    } else {
      Serial.println(F("LED off"));
      digitalWrite(led, LOW); // changed from LOW to HIGH
    }


  void checkForButtonPress() {
// read the button pins
    int button0state = digitalRead(button0);
    int button1state = digitalRead(button1);

    if (oldbutton0state != button0state) {
// button0 changed
      Serial.println("button0 pressed");
      
// button0 is pressed, write 0x01 to bluetooth to turn the motor on
      motorCharacteristicwatch0.writeValue((byte) 0x01);
    } else {
      Serial.println("button0 released");
      
// button0 is released, write 0x00 to bluetooth to turn the motor off
      motorCharacteristicwatch0.writeValue((byte) 0x00);
    }
    
    if (oldbutton1state != button1state) {
// button1 change
      Serial.println("button1 pressed");
// button1 is pressed, write 0x01 to bluetooth to turn the LED on
      ledCharacteristicwatch0.writeValue((byte) 0x01);
    } else {
      Serial.println("button1 released");
// button1 is released, write 0x00 to bluetooth turn the LED off
      ledCharacteristicwatch0.writeValue((byte) 0x00);
    }
   }
  }
