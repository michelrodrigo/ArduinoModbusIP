
// Libraries ----------------------------------------------------------
#include "SCT_plants.h"
#include <CAN.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


// Constants --------------------------------------------------------------

#define MAX_TEMP 300 // maximum temperature value
#define MIN_TEMP 10  // minimum temperature value
#define MAX_LEVEL 100 // maximum temperature value
#define MIN_LEVEL 10  // minimum temperature value
#define MAX_TIMER 30 // maximum time for the mixer
#define MIN_TIMER 2  // minimum time for the mixer

// Objects and Variables --------------------------------------------------

//Define Variables we'll be connecting to
double Setpoint, Input, Output, Setpoint2;
//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

char received_event; // stores the number of the last received event

double ts, ts2; //stores the time

int error = 0;
int aux = 0;
int partial_sp = 0; 
int partial_level = 0;
int timerMixer = 20; 
int interval = 1;

int maxLevel = 90;
int newMaxLevel = 0;

int input, output, level; //continuous time variables

int tempH1, tempH2, tempH3;


// Pins --------------------------------------------------------------
int v_in = 2; //input valve
int v_out = 3; //output valve
int outputPin   = 5;    // The pin the digital output PMW is connected to
int sensorPin   = A1;   // The pin the analog sensor is connected to
int levelSensorPin = A0;


// EEPROM ADDRESSES ------------------------------------------------------
#define TEMP_SETPOINT_ADDRESS 0
#define TEMP_H1_ADDRESS 4
#define TEMP_H2_ADDRESS 8
#define TEMP_H3_ADDRESS 12
#define MAX_LEVEL_ADDRESS 16
#define TIMER_MIXER_ADDRESS 20
#define TEMP2_SETPOINT_ADDRESS 24

// Function headers ------------------------------------------------------
void build_automata();
int get_event(int packet_size);


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


// States ---------------------------------------------------------------



// tank state
State TANK_0(&TANK_0_action, NULL, 0);
State TANK_1(&TANK_1_action, NULL, 1);
State TANK_2(&TANK_2_action, NULL, 2);
State TANK_3(&TANK_3_action, NULL, 3);



// Automata ------------------------------------------------------------

Automaton TANK(&TANK_0);

DES System;

void setup() {
  Serial.begin(9600);

  pinMode(v_in, OUTPUT);
  pinMode(v_out, OUTPUT);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  

  build_automata();

  ts = millis();
  ts2 = millis();
  read_setpoint();
  read_level_levels();
  
  Serial.println("CAN Receiver");
  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) { 
    Serial.println("Starting CAN failed!");
    while (1);
  }
  
}

void loop() {

  level = map(analogRead(levelSensorPin), 0, 1023, 0, 100);
  //Input = map(analogRead(sensorPin), 0, 1023, MIN_TEMP, MAX_TEMP);  // Read the value from the sensor
  //analogWrite(outputPin, Output);
  
  if (Serial.available()) {
    int input2 = Serial.parseInt();

    CAN.beginPacket(1);
    CAN.write(input2);
    CAN.endPacket();
  }

  
  // try to parse packet
  int packetSize = CAN.parsePacket();
  int packId = CAN.packetId();
  int event = 0;

  if (packetSize) {

      if(packId == 1){
        event = get_event(packetSize);
        if ((event % 2) == 1){
          System.trigger(event);
        }
         //Serial.println(CAN.read());
      }
      else if (packId == 2){
        aux = (int)CAN.read();        
        Input = (int)CAN.read() | (aux << 8);         
        Output = (int)CAN.read();      
      }
//      else if(packId == 3){//setpoints
//        partial_sp = (int)CAN.read();  
//        Setpoint = (int)CAN.read() | (partial_sp << 8); 
//        partial_sp = (int)CAN.read();  
//        Setpoint2 = (int)CAN.read() | (partial_sp << 8);   
//        Serial.println("Novo setpoint recebido") ;
//        update_setpoint();
//      }
      else if(packId == 4){//high level of level sensor and timer
        partial_level = (int)CAN.read();  
        maxLevel = (int)CAN.read() | (partial_level << 8); 
        partial_level = (int)CAN.read();  
        timerMixer = (int)CAN.read() | (partial_level << 8);   
        Serial.println("Novo limite nível recebido") ;
        update_level_levels();
      } 

        
      
  }

  
  if(TANK.current_state() == 1){
//    if (millis() > (ts2 + 200)) {
//       ts2 = millis();
//       level += interval;
//    }  
    if(level >= maxLevel){
       System.trigger(level_H1);
    }      
  }
  else if(TANK.current_state() == 3){
//    if (millis() > (ts2 + 200)) {
//       ts2 = millis();
//       level -= interval;
//    }  
    if(level <= 5){
       System.trigger(level_L1);
    }      
  }
  
  

  

   

    //updates the continuous time variables
    if (millis() > (ts + 800)) {
       ts = millis();
             
        input = (int)Input;
        output = (int)(Output);
        
        CAN.beginPacket(5);
        CAN.write(level);
//  
//        CAN.write(input >> 8);
//        CAN.write(input & 0XFF);
//
//        CAN.write(output);
        CAN.endPacket();
        

        lcd.clear();
        lcd.print("L: ");
        lcd.print(level, DEC);
        
        lcd.print("   T: ");
        lcd.print(input, DEC);
        //Serial.println(level+String("  ")+input+String("  ")+output+String("  "));  //look for simulation results in plotter    
    }
}
   




void TANK_0_action(){
  Serial.println("esvaziou");

  CAN.beginPacket(1);
   CAN.write(level_L1);
   CAN.endPacket();
 
}

void TANK_1_action(){
    Serial.println("enchendo");
}

void TANK_2_action(){
  Serial.println("encheu");
   
   CAN.beginPacket(1);
   CAN.write(level_H1);
   CAN.endPacket();
  
}

void TANK_3_action(){
    Serial.println("esvaziando");
}
