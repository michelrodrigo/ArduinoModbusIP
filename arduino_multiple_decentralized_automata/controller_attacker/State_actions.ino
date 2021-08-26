
void PROCESS_IDLE_action(){
   drain_out = false;
   cool = false; 
   state_process = 0; 
}

void PROCESS_PRODUCING_action(){
 
}

void VIN_0_action(){
   digitalWrite(v_in, LOW);
   valve_in = false;
}

void VIN_1_action(){    
    digitalWrite(v_in, HIGH);
    valve_in = true;      
    state_process = 1;
}

void VIN_level_H1_action(){
  
}

void VIN_open_vin_action(){
   outcoming_msg.enqueue(open_vin);
}

void VIN_close_vin_action(){
   outcoming_msg.enqueue(close_vin);
}

void VOUT_0_action(){
    digitalWrite(v_out, LOW);
    valve_out = false;
     
    //System.triggerIfPossible(finish);   
}

void VOUT_1_action(){
   digitalWrite(v_out, HIGH);
  valve_out = true;
  
  state_process = 4;
}

void VOUT_open_vout_action(){
  outcoming_msg.enqueue(open_vout);
}

void VOUT_close_vout_action(){
  outcoming_msg.enqueue(close_vout);
}

void VOUT_level_L1_action(){
  
}


void MIXER_0_action(){
    mixer = false;
}

void MIXER_1_action(){
  mixer = true;
}

void MIXER_turn_on_action(){
  outcoming_msg.enqueue(turn_on_mixer);
}

void MIXER_turn_off_action(){
  outcoming_msg.enqueue(turn_off_mixer);
}

void PUMP_0_action(){
    pump = false;
}

void PUMP_1_action(){
  pump = true;
}

void PUMP_turn_on_action(){
   outcoming_msg.enqueue(turn_on_pump);
}

void PUMP_turn_off_action(){
   outcoming_msg.enqueue(turn_off_pump);
}

void TEMP_0_action(){
  
}

void TEMP_1_action(){
  
}

void TEMP_turn_on_tcontrol_action(){
    outcoming_msg.enqueue(turn_on_tcontrol);
    state_process = 2; 
}

void TEMP_turn_off_tcontrol_action(){
    outcoming_msg.enqueue(turn_off_tcontrol);
}

void TEMP_heated_action(){
    state_process = 3;    
}

void TEMP_cooled_action(){
   
}

//Supervisors ---------------------------------------------------------------
void S2_0_action(){
    S2.enable(close_vin);    
}

void S2_1_action(){
    S2.disable(close_vin);       
}

void S3_0_action(){
    S3.enable(close_vout);    
}

void S3_1_action(){
    S3.disable(close_vout);    
}

void S4_0_action(){
  S4.disable(open_vin);
}

void S4_1_action(){  
  S4.enable(open_vin);
}

void S4_2_action(){
  S4.enable(open_vin);
}

void S5_0_action(){
  S5.disable(open_vout);
}

void S5_1_action(){
  S5.enable(open_vout);
}

void S5_2_action(){
  S5.enable(open_vout);
}

void S6_0_action(){
  S6.disable(turn_on_mixer);  
}

void S6_1_action(){
  S6.enable(turn_on_mixer);  
}

void S6_2_action(){
  S6.enable(turn_on_mixer);
}

void S7_0_action(){
  S7.disable(turn_off_mixer);
}

void S7_1_action(){
  S7.enable(turn_off_mixer);

}

void S7_2_action(){
  S7.enable(turn_off_mixer);
}

void S8_0_action(){
  S8.disable(turn_on_pump);
}

void S8_1_action(){
  S8.enable(turn_on_pump);
}

void S8_2_action(){
  
}

void S9_0_action(){
  
}

void S9_1_action(){
  S9.enable(turn_off_pump);
}

void S9_2_action(){
  S9.disable(turn_off_pump);
}

void S10_0_action(){
  S10.disable(turn_on_tcontrol);
}

void S10_1_action(){
  S10.enable(turn_on_tcontrol);
}

void S10_2_action(){
  
}

void S11_0_action(){
  S11.enable(turn_off_tcontrol);
}

void S11_1_action(){
  S11.disable(turn_off_tcontrol);
}

void S11_2_action(){
  S11.disable(turn_off_tcontrol);
}
