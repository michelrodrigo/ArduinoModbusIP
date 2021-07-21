
// Libraries ----------------------------------------------------------
#include "SCT.h"
#include <CAN.h>
#include <PID_v1.h>
#include <EEPROM.h>


// Constants --------------------------------------------------------------

#define MAX_TEMP 300 // maximum temperature value
#define MIN_TEMP 10  // minimum temperature value
#define MAX_LEVEL 100 // maximum temperature value
#define MIN_LEVEL 10  // minimum temperature value
#define MAX_TIMER 30 // maximum time for the mixer
#define MIN_TIMER 2  // minimum time for the mixer

// Pins --------------------------------------------------------------
int v_in = 2; //input valve
int v_out = 3; //output valve
int outputPin   = 5;    // The pin the digital output PMW is connected to
int sensorPin   = A1;   // The pin the analog sensor is connected to


// EEPROM ADDRESSES ------------------------------------------------------
#define TEMP_SETPOINT_ADDRESS 0
#define TEMP_H1_ADDRESS 4
#define TEMP_H2_ADDRESS 8
#define TEMP_H3_ADDRESS 12
#define MAX_LEVEL_ADDRESS 16
#define TIMER_MIXER_ADDRESS 20
#define TEMP2_SETPOINT_ADDRESS 24

char received_event;

//Define Variables we'll be connecting to
double Setpoint, Input, Output, Setpoint2;
//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void build_automata();
int get_event(int packet_size);

int tank_level = 0;

int error = 0;
int aux = 0;
int partial_sp = 0; 
int timerMixer = 20; 

int maxLevel = 90;
int newMaxLevel = 0;

int input, output;

int level;
int levelSensorPin = A0;
double ts, ts2;


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
void TEMP_0_action();
void TEMP_1_action();
void TEMP_2_action();
void TEMP_3_action();


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

// Temp states
State TEMP_0(&TEMP_0_action, NULL, 0);
State TEMP_1(&TEMP_1_action, NULL, 1);
State TEMP_2(&TEMP_2_action, NULL, 2);
State TEMP_3(&TEMP_3_action, NULL, 3);

Automaton VIN(&VIN_0);
Automaton VOUT(&VOUT_0);
Automaton TANK(&TANK_0);
Automaton MIXER(&MIXER_0);
Automaton PUMP(&PUMP_0);
Automaton TEMP(&TEMP_0);



DES System;

void setup() {
  Serial.begin(9600);

  build_automata();

   //turn the PID on
  myPID.SetMode(AUTOMATIC);
  

  pinMode(v_in, OUTPUT);
  pinMode(v_out, OUTPUT);
  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  ts = millis();
  ts2 = millis();
  read_setpoint();
  
}

void loop() {

  level = map(analogRead(levelSensorPin), 0, 1023, 0, 100);
  Input = map(analogRead(sensorPin), 0, 1023, MIN_TEMP, MAX_TEMP);  // Read the value from the sensor
  analogWrite(outputPin, Output);
  
  if (Serial.available()) {
    int input2 = Serial.parseInt();

    CAN.beginPacket(1);
    CAN.write(input2);
    CAN.endPacket();
  }

  
    // try to parse packet
  int packetSize = CAN.parsePacket();
  int packId = CAN.packetId();

  if (packetSize) {

    if(packId == 1){
       System.trigger(get_event(packetSize));
    }
    else if(packId == 3){//setpoints
      partial_sp = (int)CAN.read();  
      Setpoint = (int)CAN.read() | (partial_sp << 8); 
      partial_sp = (int)CAN.read();  
      Setpoint2 = (int)CAN.read() | (partial_sp << 8);   
      Serial.println("Novo setpoint recebido") ;
      update_setpoint();
    }
      
      
    }

    if(TANK.current_state() == 1){
      if(level >= 60){
         System.trigger(level_H1);
      }      
    }
    else if(TANK.current_state() == 3){
      if(level <= 5){
         System.trigger(level_L1);
      }      
    }

     if (millis() > (ts2 + 100)) {
       ts2 = millis();
      if(TEMP.current_state() == 1){
             myPID.Compute();      
              error = Setpoint - Input;
              if(aux/10 < timerMixer){
                aux++;
                if(abs(error) > 5){
                  aux = 0; 
                }
              }
              else{
                System.trigger(heated);
              }
              //Serial.println(Setpoint+String("  ")+input+String("  ")+output+String("  "));  //look for simulation results in plotter
         }
         else if(TEMP.current_state() ==  2){
             Setpoint = Setpoint2;
              myPID.Compute();     
              error = Setpoint - Input;
              if(aux/10 < timerMixer){
                aux++;
                if(abs(error) > 5){
                  aux = 0; 
                }
              }
              else{
                System.trigger(cooled);
              }
              //Serial.println(Setpoint+String("  ")+input+String("  ")+output+String("  "));  //look for simulation results in plotter    
          
         }
     }

    if (millis() > (ts + 950)) {
       ts = millis();

       
        input = (int)Input;
        output = (int)(Output);
        
       CAN.beginPacket(2);
       CAN.write(level);
       //CAN.endPacket();
       //Serial.println(level);

       
       //CAN.beginPacket(3);
       CAN.write(input >> 8);
       CAN.write(input & 0XFF);
       //CAN.endPacket();

       //CAN.beginPacket(4);
       CAN.write(output);
       CAN.endPacket();
       Serial.println(Setpoint+String("  ")+input+String("  ")+output+String("  "));  //look for simulation results in plotter    
    
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

  Serial.println("TEMP");
  TEMP.add_transition(&TEMP_0, &TEMP_1, turn_on_tcontrol, NULL);
  TEMP.add_transition(&TEMP_1, &TEMP_2, heated, NULL);
  TEMP.add_transition(&TEMP_2, &TEMP_3, cooled, NULL);
  TEMP.add_transition(&TEMP_3, &TEMP_0, turn_off_tcontrol, NULL);

  System.add_plant(&VIN);
  System.add_plant(&VOUT);
  System.add_plant(&TANK);
  System.add_plant(&MIXER);
  System.add_plant(&PUMP);
  System.add_plant(&TEMP);
  
}

void update_setpoint(){

  int aux = 0;
  int value1 = Setpoint / 1000;
  aux = Setpoint - value1 * 1000;
  int value2 =  aux / 100;
  aux = aux - value2 * 100;
  int value3 = aux / 10;
  aux = aux - value3 * 10;
  EEPROM.update(TEMP_SETPOINT_ADDRESS, aux);
  EEPROM.update(TEMP_SETPOINT_ADDRESS+1, value3);
  EEPROM.update(TEMP_SETPOINT_ADDRESS+2, value2);
  EEPROM.update(TEMP_SETPOINT_ADDRESS+3, value1); 

  value1 = Setpoint2 / 1000;
  aux = Setpoint2 - value1 * 1000;
  value2 =  aux / 100;
  aux = aux - value2 * 100;
  value3 = aux / 10;
  aux = aux - value3 * 10;
  EEPROM.update(TEMP2_SETPOINT_ADDRESS, aux);
  EEPROM.update(TEMP2_SETPOINT_ADDRESS+1, value3);
  EEPROM.update(TEMP2_SETPOINT_ADDRESS+2, value2);
  EEPROM.update(TEMP2_SETPOINT_ADDRESS+3, value1);
}

void read_setpoint(){  
  Setpoint = EEPROM.read(TEMP_SETPOINT_ADDRESS) + EEPROM.read(TEMP_SETPOINT_ADDRESS+1)*10 + EEPROM.read(TEMP_SETPOINT_ADDRESS+2)*100 + EEPROM.read(TEMP_SETPOINT_ADDRESS+3)*1000; 
  Setpoint2 = EEPROM.read(TEMP2_SETPOINT_ADDRESS) + EEPROM.read(TEMP2_SETPOINT_ADDRESS+1)*10 + EEPROM.read(TEMP2_SETPOINT_ADDRESS+2)*100 + EEPROM.read(TEMP2_SETPOINT_ADDRESS+3)*1000; 
}


void update_level_levels(){
  int value1 = maxLevel / 1000;
  int aux = maxLevel - value1 * 1000;
  int value2 =  aux / 100;
  aux = aux - value2 * 100;
  int value3 = aux / 10;
  aux = aux - value3 * 10;
  EEPROM.update(MAX_LEVEL_ADDRESS, aux);
  EEPROM.update(MAX_LEVEL_ADDRESS+1, value3);
  EEPROM.update(MAX_LEVEL_ADDRESS+2, value2);
  EEPROM.update(MAX_LEVEL_ADDRESS+3, value1);  

  value1 = timerMixer / 1000;
  aux = timerMixer - value1 * 1000;
  value2 =  aux / 100;
  aux = aux - value2 * 100;
  value3 = aux / 10;
  aux = aux - value3 * 10;
  EEPROM.update(TIMER_MIXER_ADDRESS, aux);
  EEPROM.update(TIMER_MIXER_ADDRESS+1, value3);
  EEPROM.update(TIMER_MIXER_ADDRESS+2, value2);
  EEPROM.update(TIMER_MIXER_ADDRESS+3, value1);  
  
  
}
void read_level_levels(){
  maxLevel = EEPROM.read(MAX_LEVEL_ADDRESS) + EEPROM.read(MAX_LEVEL_ADDRESS+1)*10 + EEPROM.read(MAX_LEVEL_ADDRESS+2)*100 + EEPROM.read(MAX_LEVEL_ADDRESS+3)*1000; 
  timerMixer = EEPROM.read(TIMER_MIXER_ADDRESS) + EEPROM.read(TIMER_MIXER_ADDRESS+1)*10 + EEPROM.read(TIMER_MIXER_ADDRESS+2)*100 + EEPROM.read(TIMER_MIXER_ADDRESS+3)*1000; 
  
}
