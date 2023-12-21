/*
  THIS CODE IS FOR THE "SLAVE: WATCH1"
  "Hearing Watch Project"
  Written by: Evan Thebearge
  Version 0.0.2 Beta
  Last Updated: 12/21/23 - 08:38 AM
  ArduinoBLE GitHub used for referencing
*/ 

#include <ArduinoBLE.h>

// define pins and button states
const int led = D2;
const int m = D6;
const int b0 = D4;
const int b1 = D5;
int ob0s = LOW;
int ob1s = LOW;

// create WATCH service
BLEService WATCH("0000");

// characteristics for reading peripheral button status to notify master
BLEByteCharacteristic pb0n("0001", BLERead | BLENotify);
BLEByteCharacteristic pb1n("0002", BLERead | BLENotify);
// characteristics for read of button status from master
BLEByteCharacteristic mb0w("1001", BLERead | BLEWrite);
BLEByteCharacteristic mb1w("2002", BLERead | BLEWrite);

void setup() {
  
// enable lithium cell battery charging
  pinMode(P0_13, OUTPUT);  
  
// enable serial logging    
  Serial.begin(9600); 
   
// set pin mode  
  pinMode(led, OUTPUT);
  pinMode(m, OUTPUT);
  pinMode(b0, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);

// begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

// set the local name, advertised service
  Serial.println("WATCH1");
  BLE.setLocalName("WATCH1");
  BLE.setAdvertisedService(WATCH);
  
// add the characteristic to the service  
  WATCH.addCharacteristic(pb0n);
  WATCH.addCharacteristic(pb1n);
  WATCH.addCharacteristic(mb0w);
  WATCH.addCharacteristic(mb1w);

// add service
BLE.addService(WATCH);

// set the initial values for the characeristics
  pb0n.writeValue((byte)0x00);
  pb1n.writeValue((byte)0x00);
  mb0w.writeValue((byte)0x00);
  mb1w.writeValue((byte)0x00);

// start advertising
BLE.advertise();
Serial.println(("BLE WATCH1 Peripheral"));  
}

void loop() {
// enable lithium cell battery high current charging 
  digitalWrite(P0_13, HIGH);
  
// listen for BLE WATCH0 to connect:
  BLEDevice central = BLE.central();

// if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    
// print the central's MAC address:
    Serial.println(central.address());
    
// while the central is still connected to peripheral:    
    while (central.connected()) {
      
// Turn on m if master b0 is pressed
      if (mb0w.written()) {
        if (mb0w.value()) {   // any value other than 0
// master b0 is pressed, write 0x01 to turn the m on         
          Serial.println("M on");
          digitalWrite(m, HIGH);
          delay(2000);
        } else {                              // a 0 value
// master b0 is released, write 0x00 to turn the m off          
          Serial.println(F("M off"));
          digitalWrite(m, LOW);
        }
      }   
        
// Turn on LED if master b1 is pressed
      if (mb1w.written()) {
        if (mb1w.value()) {   // any value other than 0
// master b1 is pressed, write 0x01 to turn the LED on           
          Serial.println("LED on");
          digitalWrite(led, HIGH);
          delay(2000);
        } else {                              // a 0 value
// master b1 is released, write 0x00 to turn the LED off           
          Serial.println(F("LED off"));
          digitalWrite(led, LOW);
        }
      }

// Turn on master m if b0 is pressed
int b0state = digitalRead(b0);

if (ob0s != b0state) {

// b0 changed
  ob0s = b0state;
  if (b0state) {
    Serial.println("b0 pressed");
    
// b0 is pressed, write 0x01 to turn the master m on
    pb0n.writeValue((byte)0x01);
      } else {
        Serial.println("b0 released");
        
// b0 is released, write 0x00 to turn the master m off
    pb0n.writeValue((byte)0x00);   
      }
    }

// Turn on master LED if b1 is pressed
int b1state = digitalRead(b1);

if (ob1s != b1state) {

// b1 changed
  ob1s = b1state;
  if (b1state) {
    Serial.println("b1 pressed");
    
// b1 pressed, write 0x01 to turn the master LED on   
    pb1n.writeValue((byte)0x01); 
      } else {
        Serial.println("b1 released");
        
// b1 is released, write 0x00 to turn the master LED off
    pb1n.writeValue((byte)0x00);   
      }
    }    
  }
    
// when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}    
