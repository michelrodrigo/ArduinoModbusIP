

#include "Fsm.h"

#define LED1_M1   36
#define LED2_M1   34
#define LED1_M2   30
#define LED2_M2   28
#define LED1_M3   24
#define LED2_M3   22
#define LED_B0    38
#define LED_B1    32
#define LED_B2    26

#define S1_M1     52
#define S2_M1     50
#define S1_M2     48
#define S2_M2     46
#define S1_M3     44
#define S2_M3     53

//macros
#define L1M1_on()   digitalWrite(LED1_M1, HIGH)   
#define L1M1_off()  digitalWrite(LED1_M1, LOW)    
#define L2M1_on()   digitalWrite(LED2_M1, HIGH)   
#define L2M1_off()  digitalWrite(LED2_M1, LOW)  
#define L1M2_on()   digitalWrite(LED1_M2, HIGH)   
#define L1M2_off()  digitalWrite(LED1_M2, LOW)  
#define L2M2_on()   digitalWrite(LED2_M2, HIGH)   
#define L2M2_off()  digitalWrite(LED2_M2, LOW)  
#define L1M3_on()   digitalWrite(LED1_M3, HIGH)   
#define L1M3_off()  digitalWrite(LED1_M3, LOW)  
#define L2M3_on()   digitalWrite(LED2_M3, HIGH)   
#define L2M3_off()  digitalWrite(LED2_M3, LOW) 
#define LB0_on()    digitalWrite(LED_B0, HIGH)   
#define LB0_off()   digitalWrite(LED_B0, LOW) 
#define LB1_on()    digitalWrite(LED_B1, HIGH)   
#define LB1_off()   digitalWrite(LED_B1, LOW) 
#define LB2_on()    digitalWrite(LED_B2, HIGH)   
#define LB2_off()   digitalWrite(LED_B2, LOW) 

//eventos controláveis
int eventos_control[] = {1, 3, 5};
int eventos_nao_control[] = {2, 4, 6};
#define a1 eventos_control[0]
#define a2 eventos_control[1]
#define a3 eventos_control[2]
#define b1 eventos_nao_control[0]
#define b2 eventos_nao_control[1]
#define b3 eventos_nao_control[2]


State M1_0(&M1_0_action, NULL);
State M1_1(&M1_1_action, NULL);
State M1_2(&M1_2_action, NULL);

State M2_0(&M2_0_action, NULL);
State M2_1(&M2_1_action, NULL);
State M2_2(&M2_2_action, NULL);

State M3_0(&M3_0_action, NULL);
State M3_1(&M3_1_action, NULL);
State M3_2(&M3_2_action, NULL);

State G1_0(&G1_0_action, NULL);
State G1_1(&G1_1_action, NULL);

State G2_0(&G2_0_action, NULL);
State G2_1(&G2_1_action, NULL);

State G3_0(&G3_0_action, NULL);
State G3_1(&G3_1_action, NULL);

State S1_0(&S1_0_action, NULL);
State S1_1(&S1_1_action, NULL);

State S2_0(&S2_0_action, NULL);
State S2_1(&S2_1_action, NULL);





Fsm M1(&M1_0);
Fsm M2(&M2_0);
Fsm M3(&M3_0);
Fsm G1(&G1_0);
Fsm G2(&G2_0);
Fsm G3(&G3_0);


Supervisor S1(&S1_0);
Supervisor S2(&S2_0);

void setup() {

  M1.add_transition(&M1_0, &M1_1, 1, NULL);
  M1.add_transition(&M1_1, &M1_2, 2, NULL);
  M1.add_transition(&M1_2, &M1_0, 3, NULL);
  
  M2.add_transition(&M2_0, &M2_1, 1, NULL);
  M2.add_transition(&M2_1, &M2_2, 2, NULL);
  M2.add_transition(&M2_2, &M2_0, 3, NULL);
  
  M3.add_transition(&M3_0, &M3_1, 1, NULL);
  M3.add_transition(&M3_1, &M3_2, 2, NULL);
  M3.add_transition(&M3_2, &M3_0, 3, NULL);

  G1.add_transition(&G1_0, &G1_1, a1, NULL);
  G1.add_transition(&G1_1, &G1_0, b1, NULL);

  G2.add_transition(&G2_0, &G2_1, a2, NULL);
  G2.add_transition(&G2_1, &G2_0, b2, NULL);

  G3.add_transition(&G3_0, &G3_1, a3, NULL);
  G3.add_transition(&G3_1, &G3_0, b3, NULL);
  
  S1.add_transition(&S1_0, &S1_0, a1, NULL);
  S1.add_transition(&S1_0, &S1_1, b1, NULL);
  S1.add_transition(&S1_1, &S1_0, a2, NULL);

  S2.add_transition(&S2_0, &S2_0, a2, NULL);
  S2.add_transition(&S2_0, &S2_1, b2, NULL);
  S2.add_transition(&S2_1, &S2_0, a3, NULL);
  
  
  pinMode(LED1_M1, OUTPUT);
  pinMode(LED2_M1, OUTPUT);
  pinMode(LED1_M2, OUTPUT);
  pinMode(LED2_M2, OUTPUT);
  pinMode(LED1_M3, OUTPUT);
  pinMode(LED2_M3, OUTPUT);
  pinMode(LED_B0, OUTPUT);
  pinMode(LED_B1, OUTPUT);
  pinMode(LED_B2, OUTPUT);
  pinMode(S1_M1, INPUT_PULLUP);
  pinMode(S2_M1, INPUT_PULLUP);
  pinMode(S1_M2, INPUT_PULLUP);
  pinMode(S2_M2, INPUT_PULLUP);
  pinMode(S1_M3, INPUT_PULLUP);
  pinMode(S2_M3, INPUT_PULLUP);

  attachInterrupt(S1_M1, M1_s1, FALLING);
  attachInterrupt(S2_M1, M1_s2, FALLING);
  attachInterrupt(S1_M2, M2_s1, FALLING);
  attachInterrupt(S2_M2, M2_s2, FALLING);
  attachInterrupt(S1_M3, M3_s1, FALLING);
  attachInterrupt(S2_M3, M3_s2, FALLING);

  Serial.begin(9600);

  S1.trigger(a1);
  S2.trigger(a2);
  
}

void loop() {
  
  if(!verifica_evento(a1)) G1.trigger(a1);
  if(!verifica_evento(a2)) G2.trigger(a2);
  if(!verifica_evento(a3)) G3.trigger(a3);
  
  delay(1000);
 

}
