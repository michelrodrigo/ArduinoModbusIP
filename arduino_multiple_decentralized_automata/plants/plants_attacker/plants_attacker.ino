/*
  Modbus-Arduino Example - Test Led (Modbus IP ESP8266)
  Control a Led on GPIO0 pin using Write Single Coil Modbus Function 
  Original library
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino

  Current version
  (c)2017 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266
*/


#include "SCT_plants.h"
#include <CAN.h>

//Modbus Registers Offsets
const int LED_COIL = 100;
const int STATE_LEVEL_IREG = 14;
//Used Pins
const int ledPin = 13; //GPIO0

// Events ---------------------------------------------------------------
int controllable_events[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
int uncontrollable_events[] = {2, 4, 6, 8, 10, 12, 14};
#define open_vin        controllable_events[0]
#define close_vin       controllable_events[1]
#define open_vout       controllable_events[2]
#define close_vout      controllable_events[3]
#define init_system     controllable_events[4]
#define turn_on_mixer   controllable_events[5]
#define turn_off_mixer  controllable_events[6]
#define turn_on_pump    controllable_events[7]
#define turn_off_pump   controllable_events[8]
#define turn_on_tcontrol  controllable_events[9]
#define turn_off_tcontrol controllable_events[10]

#define level_H1        uncontrollable_events[0]
#define level_L1        uncontrollable_events[1]
#define full            uncontrollable_events[2]
#define heated          uncontrollable_events[3]
#define cooled          uncontrollable_events[4]
#define empty           uncontrollable_events[5]
#define process_start   uncontrollable_events[6]

#define NUM_C_EVENTS 11
#define NUM_U_EVENTS 7

//ModbusIP object
//ModbusIP mb;

int a = 0;



  
void setup() {
  Serial.begin(9600);


 Serial.println("CAN Receiver");
  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) { 
    Serial.println("Starting CAN failed!");
    while (1);
  }
 
}
 
void loop() {
   //Call once inside loop() - all magic here
   int packetSize = CAN.parsePacket();
   int packId = CAN.packetId();

  if (packetSize) {
    if(packId == 1) {
      // only print packet data for non-RTR packets
      while (CAN.available()) {
        Serial.println((int)CAN.read());
      }
    }

  
    }

   
 

  if (Serial.available()) {
    int input2 = Serial.parseInt();

    CAN.beginPacket(1);
    CAN.write(input2);
    CAN.endPacket();
    Serial.println(String("Sent: ") + input2);

  }
  
}
