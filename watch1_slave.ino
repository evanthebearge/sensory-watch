/*
 * THIS CODE IS FOR THE "SLAVE: WATCH1"
 "Hearing Watch Project"
 Software borrowed from: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
 Rewritten by: Evan Thebearge
 Version: 0.0.1 Alpha
 Updated: 11/1/22 - 10:38 AM
 TO-DO:
  HEX VALUE FOR BLE SERVICES
  HEX VALUE FOR BLE CHARACTERISTIC?
  CONTROLLING CODE FOR LED,MOTOR
  CONTROLLING CODE FOR BUTTON 0,1
  DEFINE INPUT VARIABLE (0,1,2,4,8)
  MAKE BI-DIRECTIONAL COMMUNICATION SLAVE + MASTER
 */

#include <ArduinoBLE.h>
 
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth LED Service
BLEService motorService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth motor Service
BLEService button0Service("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth button0 Service
BLEService button1Service("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth button1 Service
 
// Bluetooth LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
 
const int led = D3; // pin to use for the LED
const int motor = D4; // pin to use for the motor
const int button0 = D0; // pin to use for button0 INPUT
const int button1 = D1; // pin to use for button1 INPUT
 
void setup() {
  // enable battery charging
  pinMode P0_13, OUTPUT);
  }
  
  Serial.begin(9600);
  while (!Serial);
 
  // set pins to input/output mode
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(button0, INPUT);
  pinMode(button1, INPUT);
 
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth Low Energy module failed!");
 
    while (1);
  }
 
  // set advertised local name and service UUID:
  BLE.setLocalName("WATCH0");
  BLE.setAdvertisedService(ledService);
  BLE.setAdvertisedService(motorService);
  BLE.setAdvertisedService(button0Service);
  BLE.setAdvertisedService(button1Service);
 
  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);
  motorService.addCharacteristic(switchCharacteristic);
  button0Service.addCharacteristic(switchCharacteristic);
  button1Service.addCharacteristic(switchCharacteristic);
 
  // add service
  BLE.addService(ledService);
  BLE.addService(motorService);
  BLE.addService(button0Service);
  BLE.addService(buton1Service);
 
  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);
 
  // start advertising
  BLE.advertise();
 
  Serial.println("BLE LED Peripheral");
  Serial.println("BLE Motor Peripheral");
  Serial.println("BLE Button0 Peripheral");
  Serial.println("BLE Button1 Peripheral");
}
 
void loop() {

  // battery high charging current (100mA)
  digitalWrite(P0_13, LOW);
  }
  
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();
 
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
 
    // while the central is still connected to peripheral:
  while (central.connected()) {
        if (switchCharacteristic.written()) {
          if (switchCharacteristic.value()) {   
            Serial.println("LED on");
            digitalWrite(led, LOW); // changed from HIGH to LOW       
          } else {                              
            Serial.println(F("LED off"));
            digitalWrite(led, HIGH); // changed from LOW to HIGH     
          }
        }
      }
 
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
