void build_automata(){

  

 

   Serial.println("MIXER");
  MIXER.addTransition(&MIXER_0, &MIXER_1, turn_on_mixer, NULL);
  MIXER.addTransition(&MIXER_1, &MIXER_0, turn_off_mixer, NULL);
  MIXER.addTransition(&MIXER_1, &MIXER_0, reset, NULL);
  MIXER.addTransition(&MIXER_0, &MIXER_0, reset, NULL);
  
  Serial.println("PUMP");
  PUMP.addTransition(&PUMP_0, &PUMP_1, turn_on_pump, NULL);
  PUMP.addTransition(&PUMP_1, &PUMP_0, turn_off_pump, NULL);
  PUMP.addTransition(&PUMP_1, &PUMP_0, reset, NULL);
  PUMP.addTransition(&PUMP_0, &PUMP_0, reset, NULL);

  Serial.println("TEMP");
  TEMP.addTransition(&TEMP_0, &TEMP_1, turn_on_tcontrol, NULL);
  TEMP.addTransition(&TEMP_1, &TEMP_1, heated, &TEMP_heated_action);
  TEMP.addTransition(&TEMP_1, &TEMP_1, cooled, &TEMP_cooled_action);
  TEMP.addTransition(&TEMP_1, &TEMP_0, turn_off_tcontrol, NULL);
  TEMP.addTransition(&TEMP_1, &TEMP_0, reset, NULL);
  TEMP.addTransition(&TEMP_0, &TEMP_0, reset, NULL);

  
  System.add_plant(&MIXER);
  System.add_plant(&PUMP);
  System.add_plant(&TEMP);
  
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
  int value1 = Setpoint1 / 1000;
  aux = Setpoint1 - value1 * 1000;
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
  Setpoint1 = EEPROM.read(TEMP_SETPOINT_ADDRESS) + EEPROM.read(TEMP_SETPOINT_ADDRESS+1)*10 + EEPROM.read(TEMP_SETPOINT_ADDRESS+2)*100 + EEPROM.read(TEMP_SETPOINT_ADDRESS+3)*1000; 
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
