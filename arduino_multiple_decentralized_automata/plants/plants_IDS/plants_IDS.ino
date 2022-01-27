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
int uncontrollable_events[] = {2, 4, 6, 8, 10, 12, 14, 16};
#define open_vin        controllable_events[0] // 1
#define close_vin       controllable_events[1] // 3
#define open_vout       controllable_events[2] // 5
#define close_vout      controllable_events[3] // 7
#define init            controllable_events[4] // 9
#define turn_on_mixer   controllable_events[5] // 11
#define turn_off_mixer  controllable_events[6] // 13
#define turn_on_pump    controllable_events[7] // 15
#define turn_off_pump   controllable_events[8] // 17
#define turn_on_tcontrol  controllable_events[9] // 19
#define turn_off_tcontrol controllable_events[10] // 21

#define level_H1        uncontrollable_events[0] // 2
#define level_L1        uncontrollable_events[1] // 4
#define reset           uncontrollable_events[2] // 6
#define heated          uncontrollable_events[3] // 8
#define cooled          uncontrollable_events[4] // 10
#define empty           uncontrollable_events[5] // 12
#define process_start   uncontrollable_events[6] // 14
#define finish          uncontrollable_events[7] // 16

#define NUM_C_EVENTS 11
#define NUM_U_EVENTS 7


//ModbusIP object
//ModbusIP mb;

int a = 0;

int attack_mode = 0;
int last_event = 0;
long ts; // stores the time
int cont = 0;

#define Idle        0
#define Filling     1
#define Heating     2
#define Cooling     3
#define Draining    4  

// Process model
State PROCESS_IDLE(NULL, NULL, 0);
State PROCESS_PRODUCING(NULL, NULL, 1);

// Input valve states
State VIN_0(NULL, NULL, 0);
State VIN_1(NULL, NULL, 1);

// Output valve states
State VOUT_0(NULL, NULL, 0);
State VOUT_1(NULL, NULL, 1);

// Mixer states
State MIXER_0(NULL, NULL, 0);
State MIXER_1(NULL, NULL, 1);

// Pump states
State PUMP_0(NULL, NULL, 0);
State PUMP_1(NULL, NULL, 1);

// Temp states
State TEMP_0(NULL, NULL, 0);
State TEMP_1(NULL, NULL, 1);

// Supervisor of specification E2 - states
State S2_0(NULL, NULL, 0);
State S2_1(NULL, NULL, 1);

// Supervisor of specification E3 - states
State S3_0(NULL, NULL, 0);
State S3_1(NULL, NULL, 1);

// Supervisor of specification E4 - states
State S4_0(NULL, NULL, 0);
State S4_1(NULL, NULL, 1);
State S4_2(NULL, NULL, 2);

// Supervisor of specification E5 - states
State S5_0(NULL, NULL, 0);
State S5_1(NULL, NULL, 1);
State S5_2(NULL, NULL, 2);

// Supervisor of specification E6 - states
State S6_0(NULL, NULL, 0);
State S6_1(NULL, NULL, 1);
State S6_2(NULL, NULL, 2);

// Supervisor of specification E7 - states
State S7_0(NULL, NULL, 0);
State S7_1(NULL, NULL, 1);
State S7_2(NULL, NULL, 2);

// Supervisor of specification E8 - states
State S8_0(NULL, NULL, 0);
State S8_1(NULL, NULL, 1);
State S8_2(NULL, NULL, 2);

// Supervisor of specification E9 - states
State S9_0(NULL, NULL, 0);
State S9_1(NULL, NULL, 1);
State S9_2(NULL, NULL, 2);

// Supervisor of specification E10 - states
State S10_0(NULL, NULL, 0);
State S10_1(NULL, NULL, 1);
State S10_2(NULL, NULL, 2);

// Supervisor of specification E11 - states
State S11_0(NULL, NULL, 0);
State S11_1(NULL, NULL, 1);
State S11_2(NULL, NULL, 2);

// Automata ------------------------------------------------------------

Automaton PROCESS_SYSTEM(&PROCESS_IDLE);
Automaton VIN(&VIN_0);
Automaton VOUT(&VOUT_0);
Automaton MIXER(&MIXER_0);
Automaton PUMP(&PUMP_0);
Automaton TEMP(&TEMP_0);

Automaton S2(&S2_0);
Automaton S3(&S3_0);
Automaton S4(&S4_0);
Automaton S5(&S5_0);
Automaton S6(&S6_0);
Automaton S7(&S7_0);
Automaton S8(&S8_0);
Automaton S9(&S9_0);
Automaton S10(&S10_0);
Automaton S11(&S11_0);

DES System();
  
void setup() {
  Serial.begin(9600);



 Serial.println("CAN Receiver");
  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) { 
    Serial.println("Starting CAN failed!");
    while (1);
  }

  ts = millis();
}
 
void loop() {
   //Call once inside loop() - all magic here
   int packetSize = CAN.parsePacket();
   int packId = CAN.packetId();

  if (packetSize) {
    if(packId == 1) {
      // only print packet data for non-RTR packets
      while (CAN.available()) {
        last_event = (int)CAN.read();
        Serial.println(last_event);
      }
    }

  
    }

   if (attack_mode == 1){
      if(last_event == 1){
        if (millis() > ts + 100) {
              ts = millis();
              cont++;
        }
        if(cont >= 20){
          CAN.beginPacket(1);
          CAN.write(2);
          CAN.endPacket();
          Serial.println(String("Sent: 2") );
          attack_mode = 0;
        }
      }
   }

   
 

  if (Serial.available()) {
    int input2 = Serial.parseInt();
    Serial.println(input2);
    if (input2 == 98){
      attack_mode = 1;
    }
    else if(input2 == 99){
      attack_mode = 0;
    }

    CAN.beginPacket(1);
    CAN.write(input2);
    CAN.endPacket();
    Serial.println(String("Sent: ") + input2);

  }
  
}
