/*
  THIS CODE IS FOR THE "MASTER: WATCH0"
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

void setup() {
  
// enable lithium cell battery charging
  pinMode(P0_13, OUTPUT);
    
// enable serial logging    
  Serial.begin(9600);

// set pins to input/output mode
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(button0, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);

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

// check if it has 1 of these characteristics  
  if (!peripheralbutton0notify) {
    Serial.println("Peripheral does not have readable button characteristic!");
    peripheral.disconnect();
    return;
  } else if (!peripheralbutton0notify.canRead()) {
    Serial.println("Peripheral does not have a readable button characteristic!");
    peripheral.disconnect();
    return;
  } 

// subscribe to notifcations
   
  Serial.println("Subscribing to peripheralbutton0notify ...");
  if (!peripheralbutton0notify) {
    Serial.println("characteristic peripheralbutton0notify not found!");
    peripheral.disconnect();
    return;
  } else if (!peripheralbutton0notify.canSubscribe()) {
    Serial.println("characteristic peripheralbutton0notify not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!peripheralbutton0notify.subscribe()) {
    Serial.println("peripheralbutton0notify subscription failed!");
    peripheral.disconnect();
    return;
  }
  
  Serial.println("Subscribing to peripheralbutton1notify ...");
  if (!peripheralbutton1notify) {
    Serial.println("characteristic peripheralbutton1notify not found!");
    peripheral.disconnect();
    return;
  } else if (!peripheralbutton1notify.canSubscribe()) {
    Serial.println("characteristic peripheralbutton0notify not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!peripheralbutton1notify.subscribe()) {
    Serial.println("peripheralbutton1notify subscription failed!");
    peripheral.disconnect();
    return;
  }  
  
// While the WATCH1 (peripheral) is connected
  while (peripheral.connected()) {
      
// check if slave button0 has been updated
    if (peripheralbutton0notify.valueUpdated()) {
      
// yes, get the value, characteristic is 1 byte so use byte value
      byte value = 0;
      peripheralbutton0notify.readValue(value);
      if (value & 0x01) {
// slave button0 is pressed, turn the motor on   
        Serial.println("Motor on");
        digitalWrite(motor, HIGH);
        delay(2000);
        } else {                              // a 0 value
// slave button0 is released, turn the motor off          
          Serial.println(F("Motor off"));
          digitalWrite(motor, LOW);        
      }
    }

// check if slave button1 has been updated
    if (peripheralbutton1notify.valueUpdated()) {
      
// yes, get the value, characteristic is 1 byte so use byte value
      byte value = 0;
      peripheralbutton1notify.readValue(value);
      if (value & 0x01) {
// slave button0 is pressed, turn the LED on   
        Serial.println("LED on");
        digitalWrite(led, HIGH);
        delay(2000);
        } else {                              // a 0 value
// slave button0 is released, turn the LED off          
          Serial.println(F("LED off"));
          digitalWrite(led, LOW);        
      }
    }

// Turn on slave motor if button0 is pressed
int button0state = digitalRead(button0);

if (oldbutton0state != button0state) {

// button0 changed
  oldbutton0state = button0state;
  if (button0state) {
    Serial.println("button0 pressed");
    
// button0 is pressed, write 0x01 to turn the slave motor on
    masterbutton0write.writeValue((byte)0x01);
      } else {
        Serial.println("button0 released");
        
// button0 is released, write 0x00 to turn the slave motor off
    masterbutton0write.writeValue((byte)0x00);   
      }
    }

// Turn on slave LED if button1 is pressed
int button1state = digitalRead(button1);

if (oldbutton1state != button1state) {

// button1 changed
  oldbutton1state = button1state;
  if (button1state) {
    Serial.println("button1 pressed");
    
// button1 pressed, write 0x01 to turn the slave LED on   
    masterbutton1write.writeValue((byte)0x01); 
      } else {
        Serial.println("button1 released");
        
// button1 is released, write 0x00 to turn the slave LED off
    masterbutton1write.writeValue((byte)0x00);   
      }
    }    
  }
  
  Serial.println("WATCH1 disconnected");
}
