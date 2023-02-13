/*
  THIS CODE IS FOR THE "SLAVE: WATCH1"
  "Hearing Watch Project"
  Written by: Evan Thebearge
  Version 0.0.1 Beta
  Last Updated: 2/13/23 - 11:00 AM
  ArduinoBLE GitHub used for referencing
*/ 

#include <ArduinoBLE.h>

// define pins and button states
const int led = D2;
const int motor = D6;
const int button0 = D4;
const int button1 = D5;
int oldbutton0state = LOW;
int oldbutton1state = LOW;

// create WATCH service
BLEService WATCH("0000");

// characteristics for reading peripheral button status to notify master
BLEByteCharacteristic peripheralbutton0notify("0001", BLERead | BLENotify);
BLEByteCharacteristic peripheralbutton1notify("0002", BLERead | BLENotify);
// characteristics for read of button status from master
BLEByteCharacteristic masterbutton0write("1001", BLERead | BLEWrite);
BLEByteCharacteristic masterbutton1write("2002", BLERead | BLEWrite);

void setup() {
  
// enable lithium cell battery charging
  pinMode(P0_13, OUTPUT);  
  
// enable serial logging    
  Serial.begin(9600); 
   
// set pin mode  
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(button0, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);

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
  WATCH.addCharacteristic(peripheralbutton0notify);
  WATCH.addCharacteristic(peripheralbutton1notify);
  WATCH.addCharacteristic(masterbutton0write);
  WATCH.addCharacteristic(masterbutton1write);

// add service
BLE.addService(WATCH);

// set the initial values for the characeristics
  peripheralbutton0notify.writeValue((byte)0x00);
  peripheralbutton1notify.writeValue((byte)0x00);
  masterbutton0write.writeValue((byte)0x00);
  masterbutton1write.writeValue((byte)0x00);

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
      
// Turn on motor if master button0 is pressed
      if (masterbutton0write.written()) {
        if (masterbutton0write.value()) {   // any value other than 0
// master button0 is pressed, write 0x01 to turn the motor on         
          Serial.println("Motor on");
          digitalWrite(motor, HIGH);
          delay(2000);
        } else {                              // a 0 value
// master button0 is released, write 0x00 to turn the motor off          
          Serial.println(F("Motor off"));
          digitalWrite(motor, LOW);
        }
      }   
        
// Turn on LED if master button1 is pressed
      if (masterbutton1write.written()) {
        if (masterbutton1write.value()) {   // any value other than 0
// master button1 is pressed, write 0x01 to turn the LED on           
          Serial.println("LED on");
          digitalWrite(led, HIGH);
          delay(2000);
        } else {                              // a 0 value
// master button1 is released, write 0x00 to turn the LED off           
          Serial.println(F("LED off"));
          digitalWrite(led, LOW);
        }
      }

// Turn on master motor if button0 is pressed
int button0state = digitalRead(button0);

if (oldbutton0state != button0state) {

// button0 changed
  oldbutton0state = button0state;
  if (button0state) {
    Serial.println("button0 pressed");
    
// button0 is pressed, write 0x01 to turn the master motor on
    peripheralbutton0notify.writeValue((byte)0x01);
      } else {
        Serial.println("button0 released");
        
// button0 is released, write 0x00 to turn the master motor off
    peripheralbutton0notify.writeValue((byte)0x00);   
      }
    }

// Turn on master LED if button1 is pressed
int button1state = digitalRead(button1);

if (oldbutton1state != button1state) {

// button1 changed
  oldbutton1state = button1state;
  if (button1state) {
    Serial.println("button1 pressed");
    
// button1 pressed, write 0x01 to turn the master LED on   
    peripheralbutton1notify.writeValue((byte)0x01); 
      } else {
        Serial.println("button1 released");
        
// button1 is released, write 0x00 to turn the master LED off
    peripheralbutton1notify.writeValue((byte)0x00);   
      }
    }    
  }
    
// when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}    
