/*
   THIS CODE IS FOR THE "SLAVE: WATCH1"
  "Hearing Watch Project"
  Code borrowed from:
  1: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
  2: https://raw.githubusercontent.com/arduino-libraries/ArduinoBLE/master/examples/Peripheral/CallbackLED/CallbackLED.ino
  Rewritten by: Evan Thebearge, Daniel Cardone
  Version: 0.0.1 Beta
  Updated: 1/5/22 - 10:48 PM
  TO-DO:
  TEST CODE
  LOW BATTERY ALERT
  CONNECT/DISCONNECT ALERT
  MUST ONLY BE WATCH0
  PROPERLY DEFINED FUNCTIONS (WHAT BUTTONS WILL ACTUALLY DO FINAL)
*/

#include <ArduinoBLE.h>

// create services for WATCH1
BLEService ledServiceWATCH0("30000000-0000-0000-0000-000000000000");
BLEService motorServiceWATCH0("40000000-0000-0000-0000-000000000000");
BLEService ledServiceWATCH1("10000000-0000-0000-0000-000000000000");
BLEService motorServiceWATCH1("20000000-0000-0000-0000-000000000000");

// create characteristics to allow WATCH0 to read and write
BLEByteCharacteristic ledCharacteristicWATCH0("30000001-0000-0000-0000-000000000000", BLERead | BLEWrite);
BLEByteCharacteristic motorCharacteristicWATCH0("40000001-0000-0000-0000-000000000000", BLERead | BLEWrite);
BLEByteCharacteristic ledCharacteristicWATCH1("10000001-0000-0000-0000-000000000000", BLERead | BLEWrite);
BLEByteCharacteristic motorCharacteristicWATCH1("20000001-0000-0000-0000-000000000000", BLERead | BLEWrite);

//define pins NOTE: WATCH0 IS DIFFERENT
const int led = D2;
const int motor = D5;
const int button0 = D3;
const int button1 = D4;

// variables for buttons
int oldbutton0state = LOW;
int oldbutton1state = LOW;

void setup() {
// enable lithium cell battery charging
  pinMode(P0_13, OUTPUT);

// enable serial logging    
  Serial.begin(9600);

// set pin mode  
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);

// begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  Serial.println("WATCH1");

// set the local name and UUID peripheral advertise
  BLE.setLocalName("WATCH1");
  
// set the UUID for the service this peripheral advertises
  BLE.setAdvertisedService(ledServiceWATCH0);
  BLE.setAdvertisedService(motorServiceWATCH0);
  BLE.setAdvertisedService(ledServiceWATCH1);
  BLE.setAdvertisedService(motorServiceWATCH1);
  
// add the characteristics to the service
  ledServiceWATCH0.addCharacteristic(ledCharacteristicWATCH0);
  motorServiceWATCH0.addCharacteristic(motorCharacteristicWATCH0);
  ledServiceWATCH1.addCharacteristic(ledCharacteristicWATCH1);
  motorServiceWATCH1.addCharacteristic(motorCharacteristicWATCH1);

// add services
  BLE.addService(ledServiceWATCH0);
  BLE.addService(motorServiceWATCH0);
  BLE.addService(ledServiceWATCH1);
  BLE.addService(motorServiceWATCH1);

// assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

// assign event handlers for each characteristic
  ledCharacteristicWATCH1.setEventHandler(BLEWritten, ledCharacteristicWATCH1Written);
//  motorCharacteristicWATCH1.setEventHandler(BLERead, motorCharacteristicWATCH1Read);
//  button0CharacteristicWATCH1.setEventHandler(BLEWritten, ledCharacteristicWATCH0Written);
//  button1CharacteristicWATCH1.setEventHandler(BLEWritten, motorCharacteristicWATCH0Written);
  
// set an initial value for the characteristic
  
  ledCharacteristicWATCH1.writeValue((byte)0x00);
//  motorCharacteristicWATCH1.writeValue((byte)0x00);
//  button0CharacteristicWATCH1.writeValue((byte)0x00);
//  button1CharacteristicWATCH1.writeValue((byte)0x00);

// start advertising
  BLE.advertise();

  Serial.println(("Bluetooth active, waiting for WATCH0"));
}

void loop() {

// battery high charging current (100mA)
  digitalWrite(P0_13, LOW);
  
// poll for Bluetooth Low Energy events
  BLE.poll();
}

void blePeripheralConnectHandler(BLEDevice central) {

// WATCH0 (central) connected event handler
  Serial.print("Connected event, WATCH0 (central): ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {

// WATCH0 (central) disconnected event handler
  Serial.print("Disconnected event, WATCH0 (central): ");
  Serial.println(central.address());
}

void ledCharacteristicWATCH1Written(BLEDevice central, BLECharacteristic characteristic) {
  
// WATCH0 (central) wrote new value to characteristic, update LED
  Serial.print("button1 Characteristic event, written: ");

  if (ledCharacteristicWATCH1.value()) {
    Serial.println("Bluetooth LED on");
    digitalWrite(led, HIGH);
  } else {
    Serial.println("Bluetooth LED off");
    digitalWrite(led, LOW);
  }
}

void ledCharacteristicWATCH0Read(BLEDevice central, BLECharacteristic characteristic) {
// read the button pins
    int button0state = digitalRead(button0);
    int button1state = digitalRead(button0);

    if (oldbutton0state != button0state) {
      
// button0 changed
      oldbutton0state = button0state;

      if (button0state) {
        Serial.println("button0 pressed");

// button0 is pressed, write 0x01 to turn the LED on
        ledCharacteristicWATCH1.writeValue((byte)0x01);
      } else {
        Serial.println("button0 released");

// button0 is released, write 0x00 to turn the LED off
        ledCharacteristicWATCH1.writeValue((byte)0x00);
      }
    }
  }
