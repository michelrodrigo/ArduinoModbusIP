void add_modbus_registers(){
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
    mb.addIsts(PUMP_STATUS);
}

void build_automata(){
  //Transition declaration
  Serial.println("PROCESS");
  PROCESS.add_transition(&PROCESS_0, &PROCESS_1, process_start, NULL);
  PROCESS.add_transition(&PROCESS_1, &PROCESS_2, level_H1, NULL);
  PROCESS.add_transition(&PROCESS_2, &PROCESS_3, heated, NULL);
  PROCESS.add_transition(&PROCESS_3, &PROCESS_4, cooled, NULL);
  PROCESS.add_transition(&PROCESS_4, &PROCESS_0, level_L1, NULL);
  
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
  

  //each supervisor needs a init event that is executed upon initialization. This way
  //the on enter function of the initial state is executed and the enablements/disablements are set 
  Serial.println("S1");
  S1.add_transition(&S1_0, &S1_0, init, NULL); 
  S1.add_transition(&S1_0, &S1_1, level_L1, NULL); 
  S1.add_transition(&S1_1, &S1_0, close_vout, NULL); 
  S1.add_transition(&S1_1, &S1_2, open_vin, NULL); 
  S1.add_transition(&S1_2, &S1_0, close_vout, NULL); 
  
 
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

  Serial.println("S4");
  S4.add_transition(&S4_0, &S4_0, init, NULL); 
  S4.add_transition(&S4_0, &S4_1, process_start, NULL); 
  S4.add_transition(&S4_1, &S4_0, open_vin, NULL); 

  Serial.println("S5");
  S5.add_transition(&S5_0, &S5_0, init, NULL); 
  S5.add_transition(&S5_0, &S5_1, cooled, NULL); 
  S5.add_transition(&S5_1, &S5_0, open_vout, NULL); 

  Serial.println("S6");
  S6.add_transition(&S6_0, &S6_0, init, NULL); 
  S6.add_transition(&S6_0, &S6_1, level_H1, NULL); 
  S6.add_transition(&S6_0, &S6_1, cooled, NULL); 
  S6.add_transition(&S6_1, &S6_0, cooled, NULL); 
  S6.add_transition(&S6_1, &S6_0, turn_on_mixer, NULL); 
  S6.add_transition(&S6_1, &S6_0, turn_off_mixer, NULL); 
  S6.add_transition(&S6_1, &S6_2, level_L1, NULL); 
  S6.add_transition(&S6_2, &S6_0, level_H1, NULL); 

  Serial.println("S7");
  S7.add_transition(&S7_0, &S7_0, init, NULL); 
  S7.add_transition(&S7_0, &S7_1, cooled, NULL); 
  S7.add_transition(&S7_0, &S7_1, heated, NULL); 
  S7.add_transition(&S7_1, &S7_0, cooled, NULL);
  S7.add_transition(&S7_1, &S7_0, turn_on_pump, NULL);
  S7.add_transition(&S7_1, &S7_0, turn_off_pump, NULL);
  S7.add_transition(&S7_1, &S7_2, level_L1, NULL);
  S7.add_transition(&S7_2, &S7_0, heated, NULL);

  Serial.println("S8");
  S8.add_transition(&S8_0, &S8_0, init, NULL); 
  S8.add_transition(&S8_0, &S8_1, level_H1, NULL); 
  S8.add_transition(&S8_0, &S8_1, turn_off_tcontrol, NULL); 
  S8.add_transition(&S8_1, &S8_0, level_H1, NULL); 
  S8.add_transition(&S8_1, &S8_0, turn_off_tcontrol, NULL); 
  
  
  System.add_plant(&PROCESS);
  System.add_plant(&VIN);
  System.add_plant(&VOUT);
  System.add_plant(&TANK);
  System.add_plant(&MIXER);
  System.add_plant(&PUMP);
  System.add_plant(&TEMP);
  System.add_supervisor(&S1);
  System.add_supervisor(&S2);
  System.add_supervisor(&S3);
  System.add_supervisor(&S4);
  System.add_supervisor(&S5);
  System.add_supervisor(&S6);
  System.add_supervisor(&S7);
  System.add_supervisor(&S8);
  
  
}

int get_event(){
  int id = CAN.packetId();
  int received_event;
  int event;
  int aux;

  
   received_event = (int)CAN.read();
  Serial.println(String("Received event: ") + received_event);

  return received_event;

  
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
   mb.Ists(PUMP_STATUS, pump);
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
