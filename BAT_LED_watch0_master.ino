/*
  THIS CODE IS FOR THE "MASTER: WATCH0"
  "Hearing Watch Project"
  Written by: Evan Thebearge
  Version 0.0.2 Beta
  Last Updated: 12/22/23 - 10:31 AM
  SCRIPT EDITS INCLUDE BATTERY INFORMATION AND LED CONNECT STATUS: STATUS LED ADDED NOT FUNCTION
  ArduinoBLE GitHub used for referencing
*/  

#include <ArduinoBLE.h>

// define pins and button states
const int led = D2;
const int m = D6;
const int b0 = D4;
const int b1 = D5;
const int aled0 = LEDR // IS FOR EXAMPLE activity led; BUILT_IN LED
const int aled1 = LEDB // IS FOR EXAMPLE activity led; BUILT_IN LED
int ob0s = LOW;
int ob1s = LOW;

void setup() {
  
// enable lithium cell battery charging & battery voltage monitoring
  pinMode(P0_13, OUTPUT);
  pinMode(P0_14, OUTPUT);  
// set activity led to output mode 
  pinMode(aled0, OUTPUT);
  pinMode(aled1, OUTPUT);
// enable serial logging    
  Serial.begin(9600);

// set pins to input/output mode
  pinMode(led, OUTPUT);
  pinMode(m, OUTPUT);
  pinMode(b0, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);

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
  digitalWrite(P0_13, LOW);
  
  
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
  BLECharacteristic pb0n = peripheral.characteristic("0001");
  BLECharacteristic pb1n = peripheral.characteristic("0002");
  // characteristics for read of button status from master
  BLECharacteristic mb0w = peripheral.characteristic("1001");
  BLECharacteristic mb1w = peripheral.characteristic("2002");

// check if it has 1 of these characteristics  
  if (!pb0n) {
    Serial.println("Peripheral does not have readable button characteristic!");
    peripheral.disconnect();
    return;
  } else if (!pb0n.canRead()) {
    Serial.println("Peripheral does not have a readable button characteristic!");
    peripheral.disconnect();
    return;
  } 

// subscribe to notifcations
   
  Serial.println("Subscribing to pb0n ...");
  if (!pb0n) {
    Serial.println("characteristic pb0n not found!");
    peripheral.disconnect();
    return;
  } else if (!pb0n.canSubscribe()) {
    Serial.println("characteristic pb0n not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!pb0n.subscribe()) {
    Serial.println("pb0n subscription failed!");
    peripheral.disconnect();
    return;
  }
  
  Serial.println("Subscribing to pb1n ...");
  if (!pb1n) {
    Serial.println("characteristic pb1n not found!");
    peripheral.disconnect();
    return;
  } else if (!pb1n.canSubscribe()) {
    Serial.println("characteristic pb0n not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!pb1n.subscribe()) {
    Serial.println("pb1n subscription failed!");
    peripheral.disconnect();
    return;
  }  
  
// While the WATCH1 (peripheral) is connected
  while (peripheral.connected()) {
      
// check if slave b0 has been updated
    if (pb0n.valueUpdated()) {
      
// yes, get the value, characteristic is 1 byte so use byte value
      byte value = 0;
      pb0n.readValue(value);
      if (value & 0x01) {
// slave b0 is pressed, turn the m on   
        Serial.println("M on");
        digitalWrite(m, HIGH);
        delay(2000);
        } else {                              // a 0 value
// slave b0 is released, turn the m off          
          Serial.println(F("M off"));
          digitalWrite(m, LOW);        
      }
    }

// check if slave b1 has been updated
    if (pb1n.valueUpdated()) {
      
// yes, get the value, characteristic is 1 byte so use byte value
      byte value = 0;
      pb1n.readValue(value);
      if (value & 0x01) {
// slave b0 is pressed, turn the LED on   
        Serial.println("LED on");
        digitalWrite(led, HIGH);
        delay(2000);
        } else {                              // a 0 value
// slave b0 is released, turn the LED off          
          Serial.println(F("LED off"));
          digitalWrite(led, LOW);        
      }
    }

// Turn on slave m if b0 is pressed
int b0state = digitalRead(b0);

if (ob0s != b0state) {

// b0 changed
  ob0s = b0state;
  if (b0state) {
    Serial.println("b0 pressed");
    
// b0 is pressed, write 0x01 to turn the slave m on
    mb0w.writeValue((byte)0x01);
      } else {
        Serial.println("b0 released");
        
// b0 is released, write 0x00 to turn the slave m off
    mb0w.writeValue((byte)0x00);   
      }
    }

// Turn on slave LED if b1 is pressed
int b1state = digitalRead(b1);

if (ob1s != b1state) {

// b1 changed
  ob1s = b1state;
  if (b1state) {
    Serial.println("b1 pressed");
    
// b1 pressed, write 0x01 to turn the slave LED on   
    mb1w.writeValue((byte)0x01); 
      } else {
        Serial.println("b1 released");
        
// b1 is released, write 0x00 to turn the slave LED off
    mb1w.writeValue((byte)0x00);   
      }
    }    
  }
  
  Serial.println("WATCH1 disconnected");
}
