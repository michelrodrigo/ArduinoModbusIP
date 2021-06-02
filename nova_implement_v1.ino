// Libraries ---------------------------------------------------------------
#include <PID_v1.h>
#include <Ethernet.h>
#include <Modbus.h>
#include <ModbusIP.h>

// Constants --------------------------------------------------------------

#define MAX_TEMP 300 // maximum temperature value
#define MIN_TEMP 10  // minimum temperature value

// Objects and Variables --------------------------------------------------

//ModbusIP object
ModbusIP mb;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

long ts; // stores the time



// Pins -------------------------------------------------------------------
int outputPin   = 5;    // The pin the digital output PMW is connected to
int sensorPin   = A0;   // The pin the analog sensor is connected to


//Modbus Registers Offsets (0-9999) -------------------------------------

const int SETPOINT_HREG = 1;
const int SETPOINT_IREG = 20;
const int SENSOR_IREG = 2;
const int ACTION_IREG = 3;


// Function headers ------------------------------------------------------
void update_io();


void setup () {
  Serial.begin(9600);   // Some methods require the Serial.begin() method to be called first
  pinMode(outputPin, OUTPUT);


  Setpoint = 100;
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

  ts = millis();

}

void loop () {

  Input = map(analogRead(sensorPin), 0, 1023, MIN_TEMP, MAX_TEMP);  // Read the value from the sensor
  myPID.Compute();
  analogWrite(outputPin, Output);
  Serial.println(Input+String("  ")+Setpoint+String("  ")+Output+String("  "));;  //look for simulation results in plotter

  //Call once inside loop() - all magic here
   mb.task();

   if (millis() > ts + 100) {
       ts = millis();
       update_io();
   }
}

void update_io(){

   
   Setpoint = mb.Hreg(SETPOINT_HREG);
   if(Setpoint > MAX_TEMP){
      Setpoint = MAX_TEMP;
   }
   if(Setpoint < MIN_TEMP){
      Setpoint = MIN_TEMP; 
   }

   mb.Ireg(SETPOINT_IREG, Setpoint);
   mb.Ireg(SENSOR_IREG, Input);
   mb.Ireg(ACTION_IREG, Output);  
}
