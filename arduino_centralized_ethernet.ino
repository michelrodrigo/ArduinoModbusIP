
// Libraries ---------------------------------------------------------------
#include <PID_v1.h>
#include <Ethernet.h>
#include <Modbus.h>
#include <ModbusIP.h>
#include <EEPROM.h>

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

  mb.addIreg(SENSOR_IREG);
  mb.addHreg(SETPOINT_HREG);
  mb.addIreg(SETPOINT_IREG);
  mb.addIreg(ACTION_IREG);
  mb.addIreg(TEMP_STATE_IREG);
  mb.addHreg(TEMP_H1_HREG);
  mb.addHreg(TEMP_H2_HREG);
  mb.addHreg(TEMP_H3_HREG);
  mb.addCoil(START_COIL);
  mb.addIreg(STATE_PROCESS_IREG);
  mb.addIsts(V_IN_STATUS);
  mb.addIsts(V_OUT_STATUS);
  mb.addIreg(LEVEL_IREG);
  mb.addHreg(MAX_LEVEL_HREG);
  mb.addIreg(STATE_LEVEL_IREG);
  mb.addIsts(MIXER_STATUS);
  mb.addHreg(TIMER_MIXER_HREG);
  mb.addHreg(SETPOINT2_HREG);
  mb.addIsts(PUMP);

  ts = millis();

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
       Serial.println(Setpoint+String("  ")+error+String("  ")+state_process+String("  ")+aux+String("  ")+tempH1+String("  ")+tempH2+String("  ")+tempH3+String("  "));  //look for simulation results in plotter

        // Process transitions
       if(state_process == 0 && start_process == 1){
          Output = random(10, 255);
          analogWrite(outputPin, Output);  
          state_process = 1;
          valve_in = true;      
       }
       if(state_process == 1 && stateLevel == 1){      
          state_process = 2;
          valve_in = false;
          mixer = true;
          
       }
       if(state_process == 2 && cool){
          state_process = 3;
          aux = 0;
          pump = true;
          
       }
       if(state_process == 3 && drain_out){
          state_process = 4;
          mixer = false;
          valve_out = true;
          Output = 0;
          pump = false;
       }
       if(state_process == 4 && stateLevel == 0 ){
          state_process = 0;
          valve_out = false;
          drain_out = false;
          cool = false;     
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
   if(level < 10){
      stateLevel = 0;
   }
   else if(level >= maxLevel){
      stateLevel = 1;      
   }


   

   if(start_process){
      digitalWrite(led, HIGH);
   }
   else{
      digitalWrite(led, LOW);
   }

   if(valve_in){
      digitalWrite(v_in, HIGH);
   }
   else{
      digitalWrite(v_in, LOW);
   }

   if(valve_out){
      digitalWrite(v_out, HIGH);
   }
   else{
      digitalWrite(v_out, LOW);
   }

   
}

void update_io(){

   
   newSetpoint = mb.Hreg(SETPOINT_HREG);
   if(newSetpoint != 0){
     if(newSetpoint > MAX_TEMP){
        newSetpoint = MAX_TEMP;
     }
     else if(newSetpoint < MIN_TEMP){
        newSetpoint = MIN_TEMP; 
     }
     if(newSetpoint != Setpoint){
        Setpoint = newSetpoint;
        update_setpoint();
     }
   }

   newSetpoint2 = mb.Hreg(SETPOINT2_HREG);
   if(newSetpoint2 != 0){
     if(newSetpoint2 > MAX_TEMP){
        newSetpoint2 = MAX_TEMP;
     }
     else if(newSetpoint2 < MIN_TEMP){
        newSetpoint2 = MIN_TEMP; 
     }
     if(newSetpoint2 != Setpoint2){
        Setpoint2 = newSetpoint2;
        update_setpoint();
     }
   }

   newTempH1 = mb.Hreg(TEMP_H1_HREG);
   if(newTempH1 != 0){
     if(newTempH1 > MAX_TEMP){
        newTempH1 = MAX_TEMP;
     }
     else if(newTempH1 < MIN_TEMP){
        newTempH1 = MIN_TEMP; 
     }
     if(newTempH1 != tempH1){
        tempH1 = newTempH1;
        Serial.println("Call from H1");
        update_temp_levels();
     }
   }
   
   newTempH2 = mb.Hreg(TEMP_H2_HREG);
   if(newTempH2 != 0){
     if(newTempH2 > MAX_TEMP){
        newTempH2 = MAX_TEMP;
     }
     else if(newTempH2 < MIN_TEMP){
        newTempH2 = MIN_TEMP; 
     }
     if(newTempH2 != tempH2){
        tempH2 = newTempH2;
        Serial.println("Call from H2");
        update_temp_levels();
     }
   }

   newTempH3 = mb.Hreg(TEMP_H3_HREG);
   if(newTempH3 != 0){
     if(newTempH3 > MAX_TEMP){
        newTempH3 = MAX_TEMP;
     }
     else if(newTempH3 < MIN_TEMP){
        newTempH3 = MIN_TEMP; 
     }
     if(newTempH3 != tempH3){
        tempH3 = newTempH3;
        Serial.println("Call from H3");
        update_temp_levels();
     }
   }

   newMaxLevel = mb.Hreg(MAX_LEVEL_HREG);
   if(newMaxLevel != 0){
     if(newMaxLevel > MAX_LEVEL){
        newMaxLevel = MAX_LEVEL;
     }
     else if(newMaxLevel < MIN_LEVEL){
        newMaxLevel = MIN_LEVEL; 
     }
     if(newMaxLevel != maxLevel){
        maxLevel = newMaxLevel;
        Serial.println("Call from max level");
        update_level_levels();
     }
   }

   newTimerMixer = mb.Hreg(TIMER_MIXER_HREG);
   if(newTimerMixer != 0){
     if(newTimerMixer > MAX_TIMER){
        newTimerMixer = MAX_TIMER;
     }
     else if(newTimerMixer < MIN_TIMER){
        newTimerMixer = MIN_TIMER; 
     }
     if(newTimerMixer != timerMixer){
        timerMixer = newTimerMixer;
        Serial.println("Call from timer mixer");
        update_level_levels();
     }
   }
   

   
   
   start_process = mb.Coil(START_COIL);

   mb.Ists(V_IN_STATUS, valve_in);
   mb.Ists(V_OUT_STATUS, valve_out);
   mb.Ists(PUMP, pump);
   mb.Ists(MIXER_STATUS, mixer);
  
   mb.Ireg(STATE_PROCESS_IREG, state_process);
   mb.Ireg(TEMP_STATE_IREG, temp_state);
   mb.Ireg(SETPOINT_IREG, Setpoint);
   mb.Ireg(SENSOR_IREG, Input);
   mb.Ireg(ACTION_IREG, Output); 
   mb.Ireg(SENSOR_IREG, Input);
   mb.Ireg(LEVEL_IREG, level);
   mb.Ireg(STATE_LEVEL_IREG, stateLevel);
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

void update_temp_levels(){

  Serial.println(String("Updated: ")+tempH1+String("  ")+tempH2+String("  ")+tempH3+String("  "));;  //look for simulation results in plotter
  int aux = 0;
  int value1 = tempH1 / 1000;
  aux = tempH1 - value1 * 1000;
  int value2 =  aux / 100;
  aux = aux - value2 * 100;
  int value3 = aux / 10;
  aux = aux - value3 * 10;
  EEPROM.update(TEMP_H1_ADDRESS, aux);
  EEPROM.update(TEMP_H1_ADDRESS+1, value3);
  EEPROM.update(TEMP_H1_ADDRESS+2, value2);
  EEPROM.update(TEMP_H1_ADDRESS+3, value1); 

  
  value1 = tempH2 / 1000;
  aux = tempH2 - value1 * 1000;
  value2 =  aux / 100;
  aux = aux - value2 * 100;
  value3 = aux / 10;
  aux = aux - value3 * 10;
  EEPROM.update(TEMP_H2_ADDRESS, aux);
  EEPROM.update(TEMP_H2_ADDRESS+1, value3);
  EEPROM.update(TEMP_H2_ADDRESS+2, value2);
  EEPROM.update(TEMP_H2_ADDRESS+3, value1); 

  
  value1 = tempH3 / 1000;
  aux = tempH3 - value1 * 1000;
  value2 =  aux / 100;
  aux = aux - value2 * 100;
  value3 = aux / 10;
  aux = aux - value3 * 10;
  EEPROM.update(TEMP_H3_ADDRESS, aux);
  EEPROM.update(TEMP_H3_ADDRESS+1, value3);
  EEPROM.update(TEMP_H3_ADDRESS+2, value2);
  EEPROM.update(TEMP_H3_ADDRESS+3, value1);  
}
void read_temp_levels(){
  tempH1 = EEPROM.read(TEMP_H1_ADDRESS) + EEPROM.read(TEMP_H1_ADDRESS+1)*10 + EEPROM.read(TEMP_H1_ADDRESS+2)*100 + EEPROM.read(TEMP_H1_ADDRESS+3)*1000; 
  tempH2 = EEPROM.read(TEMP_H2_ADDRESS) + EEPROM.read(TEMP_H2_ADDRESS+1)*10 + EEPROM.read(TEMP_H2_ADDRESS+2)*100 + EEPROM.read(TEMP_H2_ADDRESS+3)*1000; 
  tempH3 = EEPROM.read(TEMP_H3_ADDRESS) + EEPROM.read(TEMP_H3_ADDRESS+1)*10 + EEPROM.read(TEMP_H3_ADDRESS+2)*100 + EEPROM.read(TEMP_H3_ADDRESS+3)*1000;
  Serial.println(String("Read: ")+tempH1+String("  ")+tempH2+String("  ")+tempH3+String("  "));;  //look for simulation results in plotter 
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
