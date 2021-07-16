
// Libraries ----------------------------------------------------------
#include "SCT.h"
#include <CAN.h>

// Pins --------------------------------------------------------------
int v_in = 2; //input valve
int v_out = 3; //output valve

char received_event;


void build_automata();
int get_event(int packet_size);

int tank_level = 0;

int level;
int levelSensorPin = A0;
double ts;


// Events ---------------------------------------------------------------
int controllable_events[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23};
int uncontrollable_events[] = {2, 4, 6, 8, 10, 12, 14};
#define open_vin        controllable_events[0]
#define close_vin       controllable_events[1]
#define open_vout       controllable_events[2]
#define close_vout      controllable_events[3]
#define init            controllable_events[4]
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

#define NUM_C_EVENTS 12
#define NUM_U_EVENTS 7

void VIN_0_action();
void VIN_1_action();
void VOUT_0_action();
void VOUT_1_action();
void TANK_0_action();
void TANK_1_action();
void TANK_2_action();
void TANK_3_action();
void MIXER_0_action();
void MIXER_1_action();
void PUMP_0_action();
void PUMP_1_action();


// Input valve states
State VIN_0(&VIN_0_action, NULL, 0);
State VIN_1(&VIN_1_action, NULL, 1);

// Output valve states
State VOUT_0(&VOUT_0_action, NULL, 0);
State VOUT_1(&VOUT_1_action, NULL, 1);

// tank state
State TANK_0(&TANK_0_action, NULL, 0);
State TANK_1(&TANK_1_action, NULL, 1);
State TANK_2(&TANK_2_action, NULL, 2);
State TANK_3(&TANK_3_action, NULL, 3);

// Mixer states
State MIXER_0(&MIXER_0_action, NULL, 0);
State MIXER_1(&MIXER_1_action, NULL, 1);

// Pump states
State PUMP_0(&PUMP_0_action, NULL, 0);
State PUMP_1(&PUMP_1_action, NULL, 1);

Automaton VIN(&VIN_0);
Automaton VOUT(&VOUT_0);
Automaton TANK(&TANK_0);
Automaton MIXER(&MIXER_0);
Automaton PUMP(&PUMP_0);



DES System;

void setup() {
  Serial.begin(9600);

  build_automata();

   
  

  pinMode(v_in, OUTPUT);
  pinMode(v_out, OUTPUT);
  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  ts = millis();
  
}

void loop() {

  level = map(analogRead(levelSensorPin), 0, 1023, 0, 100);
  
  if (Serial.available()) {
    int input = Serial.parseInt();

    CAN.beginPacket(1);
    CAN.write(input);
    CAN.endPacket();
  }

    // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

       System.trigger(get_event(packetSize));
      }
      Serial.println();
    }

    if(TANK.current_state() == 1){
      tank_level++;
      delay(50);
      if(tank_level >= 60){
         System.trigger(level_H1);
      }      
    }
    else if(TANK.current_state() == 3){
      tank_level--;
      delay(50);
      if(tank_level <= 5){
         System.trigger(level_L1);
      }      
    }

    if (millis() > ts + 100) {
       ts = millis();

       CAN.beginPacket(2);
       CAN.write(level);
       CAN.endPacket();
    
}
}
   
    


// id  = 1 controllable events
// id = 2 uncontrollable events
// the first byte in the data will be 

int get_event(int packet_size){
  int id = CAN.packetId();
  int received_event;
  int event;
  int aux;

  
   received_event = (int)CAN.read();
   digitalWrite(v_out, HIGH);
   delay(100);
   digitalWrite(v_out, LOW);
  Serial.println(String("Received event: ") + received_event);

  return received_event;

  
}

void build_automata(){

  Serial.println("VIN");
  VIN.add_transition(&VIN_0, &VIN_1, open_vin, NULL);
  VIN.add_transition(&VIN_1, &VIN_0, close_vin, NULL);

  Serial.println("VOUT");
  VOUT.add_transition(&VOUT_0, &VOUT_1, open_vout, NULL);
  VOUT.add_transition(&VOUT_1, &VOUT_0, close_vout, NULL);

  Serial.println("TANK");
  TANK.add_transition(&TANK_0, &TANK_1, open_vin, NULL);  
  TANK.add_transition(&TANK_1, &TANK_2, level_H1, NULL);
  TANK.add_transition(&TANK_2, &TANK_3, open_vout, NULL);
  TANK.add_transition(&TANK_3, &TANK_0, level_L1, NULL);

   Serial.println("MIXER");
  MIXER.add_transition(&MIXER_0, &MIXER_1, turn_on_mixer, NULL);
  MIXER.add_transition(&MIXER_1, &MIXER_0, turn_off_mixer, NULL);
  
  Serial.println("PUMP");
  PUMP.add_transition(&PUMP_0, &PUMP_1, turn_on_pump, NULL);
  PUMP.add_transition(&PUMP_1, &PUMP_0, turn_off_pump, NULL);

  System.add_plant(&VIN);
  System.add_plant(&VOUT);
  System.add_plant(&TANK);
  System.add_plant(&MIXER);
  System.add_plant(&PUMP);
}
