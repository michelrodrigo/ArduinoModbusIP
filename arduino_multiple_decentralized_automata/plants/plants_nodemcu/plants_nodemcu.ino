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

#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include "SCT_plants.h"

//Modbus Registers Offsets
const int LED_COIL = 100;
const int STATE_LEVEL_IREG = 14;
//Used Pins
const int ledPin = 13; //GPIO0

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

//ModbusIP object
ModbusIP mb;

int a = 0;
int mixer_command, old_mixer_command = 0;

// Mixer states
State MIXER_0(&MIXER_0_action, NULL, 0);
State MIXER_1(&MIXER_1_action, NULL, 1);

Automaton MIXER(&MIXER_0);

DES System;
  
void setup() {
  Serial.begin(9600);
 
  WiFi.begin("Arduino", "redeArduino");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mb.server();

  pinMode(ledPin, OUTPUT);
  mb.addCoil(LED_COIL);
  mb.addIreg(STATE_LEVEL_IREG);

   Serial.println("MIXER");
  MIXER.add_transition(&MIXER_0, &MIXER_1, turn_on_mixer, NULL);
  MIXER.add_transition(&MIXER_1, &MIXER_0, turn_off_mixer, NULL);

  System.add_plant(&MIXER);
}
 
void loop() {
   //Call once inside loop() - all magic here
   mb.task();

   mixer_command = mb.Coil(LED_COIL);
   if(mixer_command != old_mixer_command){
      old_mixer_command = mixer_command;
      if(mixer_command){
        System.trigger(turn_on_mixer);  
      }
      else{
        System.trigger(turn_off_mixer);
      }
      
   }
   

   mb.Ireg(STATE_LEVEL_IREG, a);
   //Attach ledPin to LED_COIL register
   //digitalWrite(ledPin, mb.Coil(LED_COIL));
  
}
