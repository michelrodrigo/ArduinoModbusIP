#include "SCT.h"


//Event declaration
//Controllable events are assigned to odd numbers, while uncontrollable events are assigned to even numbers.
int controllable_events[] = {1, 3, 5, 7, 9};
int uncontrollable_events[] = {2, 4};
#define open_vin    controllable_events[0]
#define close_vin   controllable_events[1]
#define open_vout   controllable_events[2]
#define close_vout  controllable_events[3]
#define init        controllable_events[4]
#define level_H1       uncontrollable_events[0]
#define level_L1       uncontrollable_events[1]


//Each state can have an on enter function, that is called when the state become the current state. Here are the 
//headers to those functions
void VIN_0_action();
void VIN_1_action();
void VOUT_0_action();
void VOUT_0_action();
void TANK_0_action();
void TANK_1_action();
void S1_0_action();
void S1_1_action();
void S2_0_action();
void S2_1_action();
void S3_0_action();
void S3_1_action();



//State declaration
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

// Supervisor of specification E1 - states
State S1_0(&S1_0_action, NULL, 0);
State S1_1(&S1_1_action, NULL, 1);

// Supervisor of specification E2 - states
State S2_0(&S2_0_action, NULL, 0);
State S2_1(&S2_1_action, NULL, 1);

// Supervisor of specification E3 - states
State S3_0(&S3_0_action, NULL, 0);
State S3_1(&S3_1_action, NULL, 1);


//Automata declaration
Automaton VIN(&VIN_0);
Automaton VOUT(&VOUT_0);
Automaton TANK(&TANK_0);


Supervisor S1(&S1_0);
Supervisor S2(&S2_0);
Supervisor S3(&S3_0);

DES System;
int incomingByte = 0; // for incoming serial data

void setup() {

  Serial.begin(9600);

  //Transition declaration
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

  //each supervisor needs a init event that is executed upon initialization. This way
  //the on enter function of the initial state is executed and the enablements/disablements are set 
  Serial.println("S1");
  S1.add_transition(&S1_0, &S1_0, init, NULL); 
  S1.add_transition(&S1_0, &S1_0, open_vin, NULL);
  S1.add_transition(&S1_0, &S1_0, open_vout, NULL);
  S1.add_transition(&S1_0, &S1_1, close_vin, NULL);
  S1.add_transition(&S1_0, &S1_1, close_vin, NULL);
  S1.add_transition(&S1_0, &S1_1, level_H1, NULL);
  S1.add_transition(&S1_0, &S1_1, level_L1, NULL);
  S1.add_transition(&S1_1, &S1_0, level_H1, NULL);
  S1.add_transition(&S1_1, &S1_0, level_L1, NULL);
  S1.add_transition(&S1_1, &S1_0, close_vin, NULL);
  S1.add_transition(&S1_1, &S1_0, close_vout, NULL);

  Serial.println("S2");
  S2.add_transition(&S2_0, &S2_0, init, NULL); 
  S2.add_transition(&S2_0, &S2_0, close_vin, NULL);
  S2.add_transition(&S2_0, &S2_0, open_vout, NULL);
  S2.add_transition(&S2_0, &S2_0, close_vout, NULL);
  S2.add_transition(&S2_0, &S2_0, level_L1, NULL);
  S2.add_transition(&S2_0, &S2_1, open_vin, NULL);
  S2.add_transition(&S2_1, &S2_1, close_vout, NULL);
  S2.add_transition(&S2_1, &S2_0, level_H1, NULL);

  Serial.println("S3");
  S3.add_transition(&S3_0, &S3_0, init, NULL); 
  S3.add_transition(&S3_0, &S3_0, close_vin, NULL);
  S3.add_transition(&S3_0, &S3_0, open_vin, NULL);
  S3.add_transition(&S3_0, &S3_0, close_vout, NULL);
  S3.add_transition(&S3_0, &S3_0, level_H1, NULL);
  S3.add_transition(&S3_0, &S3_1, open_vout, NULL);
  S3.add_transition(&S3_1, &S3_1, close_vin, NULL);
  S3.add_transition(&S3_1, &S3_0, level_L1, NULL);
  

  System.add_plant(&VIN);
  System.add_plant(&VOUT);
  System.add_plant(&TANK);
  System.add_supervisor(&S1);
  System.add_supervisor(&S2);
  System.add_supervisor(&S3);
  
  
  

  Serial.println("Initializing...");
  S1.trigger(init); //executes initial event for the supervisor
  S2.trigger(init); //executes initial event for the supervisor
  S3.trigger(init); //executes initial event for the supervisor
  
}

void loop() {

  
    // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  //if(!is_disabled_global(a1)) G1.trigger(a1);
  //if(!is_disabled_global(a2)) G2.trigger(a2);
  //if(!is_disabled_global(a3)) G3.trigger(a3);
  }

  switch(incomingByte){
    case(97): //a
      System.trigger_if_possible(open_vin);
      //if(!is_disabled_global(open_vin)) VIN.trigger(open_vin);
      break;
    case(98): //b
      System.trigger_if_possible(close_vin);
      //if(!is_disabled_global(close_vin)) VIN.trigger(close_vin);
      break;
    case(99): //c
      System.trigger_if_possible(open_vout);
      //if(!is_disabled_global(open_vout)) VOUT.trigger(open_vout);
      break;
    case(100): //d
      System.trigger_if_possible(close_vout);
      //if(!is_disabled_global(close_vout)) VOUT.trigger(close_vout);
      break;
    case(101): //e
      System.trigger_if_possible(level_L1);
      //TANK.trigger(level_L1);
      break;
    case(102): //f
      System.trigger_if_possible(level_H1);
      //TANK.trigger(level_H1);
      break;
  }
  incomingByte = 255;
}

//retorna 1 se o evento estiver desabilitado por pelo menos um supervisor e 
//retorna 0 caso contrário

//returns true if the given event is disabled by at least one supervisor
//returns false, otherwise
//bool is_disabled_global(int event)
//{
//  return S1.is_disabled(event) || S2.is_disabled(event) || S3.is_disabled(event);
//}
//
//void supervisor_trigger(int event){
//  S1.trigger(event);
//  S2.trigger(event);
//  S3.trigger(event);
//  
//}
