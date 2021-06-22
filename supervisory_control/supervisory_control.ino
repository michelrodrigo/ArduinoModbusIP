#include "Fsm.h"



//eventos controláveis
int controllable_events[] = {1, 3, 5, 7, 9};
int uncontrollable_events[] = {2, 4};
#define open_vin    controllable_events[0]
#define close_vin   controllable_events[1]
#define open_vout   controllable_events[2]
#define close_vout  controllable_events[3]
#define init        controllable_events[4]
#define full        uncontrollable_events[0]
#define empty       uncontrollable_events[1]

void VIN_0_action();
void VIN_1_action();
void VOUT_0_action();
void VOUT_0_action();
void HI_LEVEL_0_action();
void LO_LEVEL_0_action();
void S1_0_action();
void S1_1_action();
void S1_2_action();
void S1_3_action();


// Input valve states
State VIN_0(&VIN_0_action, NULL);
State VIN_1(&VIN_1_action, NULL);

// Output valve states
State VOUT_0(&VOUT_0_action, NULL);
State VOUT_1(&VOUT_1_action, NULL);

// High level sensor state
State HI_LEVEL_0(&HI_LEVEL_0_action, NULL);

// Low level sensor state
State LO_LEVEL_0(&LO_LEVEL_0_action, NULL);

// Supervisor of specification E1 - states
State S1_0(&S1_0_action, NULL);
State S1_1(&S1_1_action, NULL);
State S1_2(&S1_2_action, NULL);
State S1_3(&S1_3_action, NULL);


Fsm VIN(&VIN_0);
Fsm VOUT(&VOUT_0);
Fsm HI_LEVEL(&HI_LEVEL_0);
Fsm LO_LEVEL(&LO_LEVEL_0);

Supervisor S1(&S1_0);

int incomingByte = 0; // for incoming serial data

void setup() {

  VIN.add_transition(&VIN_0, &VIN_1, open_vin, NULL);
  VIN.add_transition(&VIN_1, &VIN_0, close_vin, NULL);  
  
  VOUT.add_transition(&VOUT_0, &VOUT_1, open_vout, NULL);
  VOUT.add_transition(&VOUT_1, &VOUT_0, close_vout, NULL);
  
  HI_LEVEL.add_transition(&HI_LEVEL_0, &HI_LEVEL_0, full, NULL);
  
  LO_LEVEL.add_transition(&LO_LEVEL_0, &LO_LEVEL_0, empty, NULL);

  S1.add_transition(&S1_0, &S1_0, init, NULL);
  S1.add_transition(&S1_0, &S1_1, open_vin, NULL);
  S1.add_transition(&S1_1, &S1_2, close_vin, NULL);
  S1.add_transition(&S1_2, &S1_3, open_vout, NULL);
  S1.add_transition(&S1_3, &S1_0, close_vout, NULL);
  
  
  
  Serial.begin(9600);

  S1.trigger(init);
  
}

void loop() {


    // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  //if(!verifica_evento(a1)) G1.trigger(a1);
  //if(!verifica_evento(a2)) G2.trigger(a2);
  //if(!verifica_evento(a3)) G3.trigger(a3);
  }

  switch(incomingByte){
    case(97): //a
      if(!verifica_evento(open_vin)) VIN.trigger(open_vin);
      break;
    case(98): //b
      if(!verifica_evento(close_vin)) VIN.trigger(close_vin);
      break;
    case(99): //c
      if(!verifica_evento(open_vout)) VOUT.trigger(open_vout);
      break;
    case(100): //d
      if(!verifica_evento(close_vout)) VOUT.trigger(close_vout);
      break;
    case(101): //e
      LO_LEVEL.trigger(empty);
      break;
    case(102): //f
      HI_LEVEL.trigger(full);
      break;
  }
  incomingByte = 255;
}
