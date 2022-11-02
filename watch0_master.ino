/*
 * THIS CODE IS FOR THE "MASTER: WATCH0"
 "Hearing Watch Project"
 Software borrowed from: https://wiki.seeedstudio.com/XIAO-BLE-Sense-Bluetooth-Usage/
 Rewritten by: Evan Thebearge
 Version: 0.0.1 Alpha
 Updated: 11/1/22 - 11:15 AM
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
 
BLEService ledService("03B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth LED Service
BLEService motorService("04B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth motor Service
BLEService button0Service("00B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth button0 Service
BLEService button1Service("01B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth button1 Service
 
BLEByteCharacteristic ledCharacteristic("03B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); // Bluetooth LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic motorCharacteristic("04B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); // Bluetooth motor Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic button0Characteristic("00B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); // Bluetooth button0 Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic button1Characteristic("01B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); // Bluetooth button1 Switch Characteristic - custom 128-bit UUID, read and writable by central
 
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
    Serial.println("starting Bluetooth module failed!");
 
    while (1);
  }
 
  // set advertised local name and service UUID:
  BLE.setLocalName("WATCH0");
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
  BLE.addService(ledService);
  BLE.addService(motorService);
  BLE.addService(button0Service);
  BLE.addService(buton1Service);
 
  // set the initial value for the characeristics:
  ledCharacteristic.writeValue(0);
  motorCharacteristic.writeValue(0);
  button0Characteristic.writeValue(0);
  button1Characteristic.writeValue(0);
 
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
        if (ledCharacteristic.written()) {
          if (ledCharacteristic.value()) {   
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
