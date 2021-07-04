
// Libraries ---------------------------------------------------------------
#include <PID_v1.h>
#include <Ethernet.h>
#include <Modbus.h>
#include <ModbusIP.h>
#include <EEPROM.h>
#include "SCT.h"

// Constants --------------------------------------------------------------

#define MAX_TEMP 300 // maximum temperature value
#define MIN_TEMP 10  // minimum temperature value
#define MAX_LEVEL 100 // maximum temperature value
#define MIN_LEVEL 10  // minimum temperature value
#define MAX_TIMER 30 // maximum time for the mixer
#define MIN_TIMER 2  // minimum time for the mixer

// Objects and Variables --------------------------------------------------

//ModbusIP object
ModbusIP mb;

//Define Variables we'll be connecting to
double Setpoint, Input, Output, Setpoint2;
int newSetpoint, newSetpoint2;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

long ts; // stores the time

int temp_state = 0; //stores the state of the temperature automaton
int tempH1 = 50; //stores the temperature that defines the events H1 and L1
int newTempH1 = 0;
int tempH2 = 100; //stores the temperature that defines the events H2 and L2
int newTempH2 = 0;
int tempH3 = 200; //stores the temperature that defines the events H3 and L3
int newTempH3 = 0;

int state_process = 0; //stores the state of the process: 0=idle, 1=filling tank, 2=heating, 3=draining out
int aux = 0; //
int level = 0; //stores the level of the tank
int maxLevel = 90;
int newMaxLevel = 0;
int stateLevel = 0;
int timerMixer = 10; 
int newTimerMixer = 0;
int error = 0;

bool start_process = false;
bool stop_process = false;
bool drain_out = false;
bool valve_in = false;
bool valve_out = false;
bool mixer = false;
bool cool = false;
bool pump = false;




// Pins -------------------------------------------------------------------
int outputPin   = 5;    // The pin the digital output PMW is connected to
int sensorPin   = A0;   // The pin the analog sensor is connected to
int led         = 6;
int v_in        = 7; // valve in
int v_out       = 8; // valve out
int levelSensorPin = A1;


//Modbus Registers Offsets (0-9999) -------------------------------------

const int SETPOINT_HREG = 1;
const int SETPOINT_IREG = 20;
const int SENSOR_IREG = 2;
const int ACTION_IREG = 3;
const int TEMP_STATE_IREG = 4;
const int TEMP_H1_HREG = 5;
const int TEMP_H2_HREG = 6;
const int TEMP_H3_HREG = 7;
const int START_COIL = 8;
const int STATE_PROCESS_IREG = 9;
const int V_IN_STATUS = 10;
const int V_OUT_STATUS = 11;
const int LEVEL_IREG = 12;
const int MAX_LEVEL_HREG = 13;
const int STATE_LEVEL_IREG = 14;
const int MIXER_STATUS = 15;
const int TIMER_MIXER_HREG = 16;
const int SETPOINT2_HREG = 17;
const int PUMP = 18;

// EEPROM ADDRESSES ------------------------------------------------------
#define TEMP_SETPOINT_ADDRESS 0
#define TEMP_H1_ADDRESS 4
#define TEMP_H2_ADDRESS 8
#define TEMP_H3_ADDRESS 12
#define MAX_LEVEL_ADDRESS 16
#define TIMER_MIXER_ADDRESS 20
#define TEMP2_SETPOINT_ADDRESS 24

// Function headers ------------------------------------------------------
void update_io();
void update_setpoint();
void read_setpoint();
void update_temp_levels();
void read_temp_levels();
void update_level_levels();
void read_level_levels();
void add_modbus_registers();
void build_automata();

//state actions
void PROCESS_0_action();
void PROCESS_1_action();
void PROCESS_2_action();
void PROCESS_3_action();
void PROCESS_4_action();
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

// Events ---------------------------------------------------------------
int controllable_events[] = {1, 3, 5, 7, 9, 11, 13, 15};
int uncontrollable_events[] = {2, 4, 6, 8, 10, 12, 14};
#define open_vin        controllable_events[0]
#define close_vin       controllable_events[1]
#define open_vout       controllable_events[2]
#define close_vout      controllable_events[3]
#define init            controllable_events[4]
#define turn_on_mixer   controllable_events[5]
#define turn_off_mixer  controllable_events[6]

#define level_H1        uncontrollable_events[0]
#define level_L1        uncontrollable_events[1]
#define full            uncontrollable_events[2]
#define heated          uncontrollable_events[3]
#define cooled          uncontrollable_events[4]
#define empty           uncontrollable_events[5]
#define process_start   uncontrollable_events[6]


#define NUM_C_EVENTS 7
#define NUM_U_EVENTS 7

 int list[]={open_vin, close_vin, open_vout, close_vout};

//int enabled_events[NUM_EVENTS];

// States ---------------------------------------------------------------

// Process states
State PROCESS_0(&PROCESS_0_action, NULL, 0);
State PROCESS_1(&PROCESS_1_action, NULL, 1);
State PROCESS_2(&PROCESS_2_action, NULL, 2);
State PROCESS_3(&PROCESS_3_action, NULL, 3);
State PROCESS_4(&PROCESS_4_action, NULL, 4);
#define Idle        0
#define Filling     1
#define Heating     2
#define Cooling     3
#define Draining    4  

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

// Output valve states
State MIXER_0(&MIXER_0_action, NULL, 0);
State MIXER_1(&MIXER_1_action, NULL, 1);

// Supervisor of specification E1 - states
State S1_0(&S1_0_action, NULL, 0);
State S1_1(&S1_1_action, NULL, 1);

// Supervisor of specification E2 - states
State S2_0(&S2_0_action, NULL, 0);
State S2_1(&S2_1_action, NULL, 1);

// Supervisor of specification E3 - states
State S3_0(&S3_0_action, NULL, 0);
State S3_1(&S3_1_action, NULL, 1);

// Supervisor of specification E4 - states
State S4_0(&S4_0_action, NULL, 0);
State S4_1(&S4_1_action, NULL, 1);

// Supervisor of specification E5 - states
State S5_0(&S5_0_action, NULL, 0);
State S5_1(&S5_1_action, NULL, 1);

// Supervisor of specification E6 - states
State S6_0(&S6_0_action, NULL, 0);
State S6_1(&S6_1_action, NULL, 1);

// Supervisor of specification E7 - states
State S7_0(&S7_0_action, NULL, 0);
State S7_1(&S7_1_action, NULL, 1);


// Automata ------------------------------------------------------------
Automaton PROCESS(&PROCESS_0);
Automaton VIN(&VIN_0);
Automaton VOUT(&VOUT_0);
Automaton TANK(&TANK_0);
Automaton MIXER(&MIXER_0);

Supervisor S1(&S1_0);
Supervisor S2(&S2_0);
Supervisor S3(&S3_0);
Supervisor S4(&S4_0);
Supervisor S5(&S5_0);
Supervisor S6(&S6_0);
Supervisor S7(&S7_0);



DES System(controllable_events, NUM_C_EVENTS, uncontrollable_events, NUM_U_EVENTS);

void setup () {
  Serial.begin(9600);   // Some methods require the Serial.begin() method to be called first
  pinMode(outputPin, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(v_in, OUTPUT);
  pinMode(v_out, OUTPUT);

  Serial.println(start_process+String("  ")+tempH1+String("  ")+tempH2+String("  ")+tempH3+String("  "));;  //look for simulation results in plotter
  read_setpoint(); //reads from EEPROM
  read_temp_levels();
  read_level_levels();
  Serial.println(start_process+String("  ")+tempH1+String("  ")+tempH2+String("  ")+tempH3+String("  "));;  //look for simulation results in plotter
   //turn the PID on
  myPID.SetMode(AUTOMATIC);

  // The media access control (ethernet hardware) address for the shield
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  // The IP address for the shield
  byte ip[] = { 192, 168, 0, 120 };
  //Config Modbus IP
  mb.config(mac, ip);
  // Add LAMP1_COIL register - Use addCoil() for digital outputs

  add_modbus_registers();

  build_automata();
  //System.setMode(3, list, 4);
  System.setMode(RANDOM, NULL, NUM_C_EVENTS);

  Serial.println("Initializing...");
  S1.trigger(init); //executes initial event for the supervisor
  S2.trigger(init); //executes initial event for the supervisor
  S3.trigger(init); //executes initial event for the supervisor
  S4.trigger(init);
  S5.trigger(init);
  S6.trigger(init);
  S7.trigger(init);
  ts = millis();

  System.updateDES();

  randomSeed(analogRead(A5));

  delay(5000);

}

void loop () {

  

  level = map(analogRead(levelSensorPin), 0, 1023, 0, 100);
  Input = map(analogRead(sensorPin), 0, 1023, MIN_TEMP, MAX_TEMP);  // Read the value from the sensor
  analogWrite(outputPin, Output);
  
 
  //Call once inside loop() - all magic here
  mb.task();

   if (millis() > ts + 100) {
       ts = millis();
       update_io();
       //Serial.println(Setpoint+String("  ")+error+String("  ")+state_process+String("  ")+aux+String("  ")+tempH1+String("  ")+tempH2+String("  ")+tempH3+String("  "));  //look for simulation results in plotter

       
       

        // Process transitions
       if(PROCESS.current_state() == Idle && start_process == 1){
          System.trigger_if_possible(process_start);
       }
       if(PROCESS.current_state() == Filling && stateLevel == 1){      
          System.trigger_if_possible(full);
          
       }
       if(PROCESS.current_state() == Heating && cool){
         System.trigger_if_possible(heated);
          
       }
       if(PROCESS.current_state() == Cooling && drain_out){
          System.trigger_if_possible(cooled);
       }
       if(PROCESS.current_state() == Draining && stateLevel == 0 ){
          System.trigger_if_possible(empty);
       }
    
       //Process states
       switch(state_process){
          case(0):
            aux = 0;
            break;
          case(1):        
            break;
          case(2):
            myPID.Compute();      
            error = Setpoint - Input;
            if(aux/10 < timerMixer){
              aux++;
              if(abs(error) > 5){
                aux = 0; 
              }
            }
            else{
              cool = true;
            }
            break;
          case(3):
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
                drain_out = true;
              }        
              break;
        
       }
       
   
   }

   // Temperature
   if(Input <= tempH1){
     temp_state = 0;
   }
   else if ((Input > tempH1) && (Input <= tempH2)){
     temp_state = 1;
   }
   else if ((Input > tempH2) && (Input <= tempH3)){
     temp_state = 2;
   }
   else if(Input > tempH3){
     temp_state = 3;
   }

   // Level
   if(level < 10 && stateLevel == 1){
      System.trigger_if_possible(level_L1);
       stateLevel = 0;
   }
   else if(level >= maxLevel && stateLevel == 0){
      System.trigger_if_possible(level_H1);   
      stateLevel = 1; 
   }


   

   if(start_process){
      digitalWrite(led, HIGH);
   }
   else{
      digitalWrite(led, LOW);
   }


   
}
