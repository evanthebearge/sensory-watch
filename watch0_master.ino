/*
  THIS CODE IS FOR THE "MASTER: WATCH0"
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

void setup() {
  
// enable lithium cell battery charging
  pinMode(P0_13, OUTPUT);
    
// enable serial logging    
  Serial.begin(9600);

// set pins to input/output mode
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);

// begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }
  
// start scanning for WATCH1
  BLE.scanForName("WATCH1");
}

void loop() {    
// enable lithium cell battery high current charging  
  digitalWrite(P0_13, HIGH);
  
// check if WATCH0 (peripheral) has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    
// discovered WATCH0 (peripheral), print out address, local name, and advertised services
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
    
// stop scanning for WATCH1
    BLE.stopScan();
    
// run WATCH scripts
    control(peripheral);  
    
// if it disconnects, restart scanning for WATCH1
    BLE.scanForName("WATCH1");
  }
} 

void control(BLEDevice peripheral) {

// connect to the WATCH1 (peripheral)
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

// discover WATCH1 (peripheral) attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }     

// retrieve the characteristics from WATCH1 and verify
  // characteristics for reading peripheral button status to notify master
  BLECharacteristic peripheralbutton0notify = peripheral.characteristic("0001");
  BLECharacteristic peripheralbutton1notify = peripheral.characteristic("0002");
  // characteristics for read of button status from master
  BLECharacteristic masterbutton0write = peripheral.characteristic("1001");
  BLECharacteristic masterbutton1write = peripheral.characteristic("2002");

// check if has 1 of these characteristics  
  if (!peripheralbutton0notify) {
    Serial.println("Peripheral does not have readable button characteristic!");
    peripheral.disconnect();
    return;
  } else if (!peripheralbutton0notify.canRead()) {
    Serial.println("Peripheral does not have a readable button characteristic!");
    peripheral.disconnect();
    return;
  }  

// While the WATCH1 (peripheral) is connected
  while (peripheral.connected()) {

// code here will be to do the following:
  // read button0,button1 status and send to watch0
  // turn motor/led on or off based on info from watch0
       
  }
  
  Serial.println("WATCH1 disconnected");
}
