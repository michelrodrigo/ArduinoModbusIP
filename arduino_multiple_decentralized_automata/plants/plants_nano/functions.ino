void build_automata(){

  Serial.println("VIN");
  VIN.addTransition(&VIN_0, &VIN_1, open_vin, NULL);
  VIN.addTransition(&VIN_1, &VIN_0, close_vin, NULL);  
  VIN.addTransition(&VIN_1, &VIN_1, level_H1, &VIN_level_H1_action);  
  VIN.addTransition(&VIN_1, &VIN_0, reset, NULL);
  VIN.addTransition(&VIN_0, &VIN_0, reset, NULL);
  
  Serial.println("VOUT");
  VOUT.addTransition(&VOUT_0, &VOUT_1, open_vout, NULL);
  VOUT.addTransition(&VOUT_1, &VOUT_0, close_vout, NULL);
  VOUT.addTransition(&VOUT_1, &VOUT_1, level_L1, &VOUT_level_L1_action);
  VOUT.addTransition(&VOUT_1, &VOUT_0, reset, NULL);
  VOUT.addTransition(&VOUT_0, &VOUT_0, reset, NULL);

  
  System.add_plant(&VIN);
  System.add_plant(&VOUT);
  

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
 
  Serial.println(String("Received event: ") + received_event);

  return received_event;

  
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
