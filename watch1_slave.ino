/*
  THIS CODE IS FOR THE "SLAVE: WATCH1"
  "Hearing Watch Project"
  Written by: Evan Thebearge
  Version 0.0.1 Beta
  Last Updated: 1/6/22 - 2:20 PM
  ArduinoBLE GitHub used for referencing
*/ 

#include <ArduinoBLE.h>

// define pins and button states NOTE: WATCH0 IS DIFFERENT
const int led = D2;
const int motor = D5;
const int button0 = D3;
const int button1 = D4;
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
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);

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
// code here will be to do the following:
  // read button0,button1 status and send to watch0
  // turn motor/led on or off based on info from watch0
 
    }
    
// when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}    
