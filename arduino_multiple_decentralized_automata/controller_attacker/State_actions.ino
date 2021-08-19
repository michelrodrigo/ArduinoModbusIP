void PROCESS_0_action(){
   Serial.println("Process: Idle");
   //System.trigger_if_possible(close_vout);
   drain_out = false;
   cool = false; 
   state_process = 0;    
}

void PROCESS_1_action(){
   Serial.println("Process: Filling");
   Output = random(10, 255);
   analogWrite(outputPin, Output);  
   //System.trigger_if_possible(open_vin);   
   state_process = 1;
}

void PROCESS_2_action(){
   Serial.println("Process: Heating");
   //System.trigger_if_possible(close_vin);
   
   state_process = 2;
}

void PROCESS_3_action(){
   Serial.println("Process: Cooling");
   aux = 0;   
   state_process = 3;
}

void PROCESS_4_action(){
   Serial.println("Process: Draining");
   
   //System.trigger_if_possible(open_vout);
   Output = 0;
   state_process = 4;
}


void PROCESS_IDLE_action(){
  Serial.println("Process in idle state");
  drain_out = false;
   cool = false; 
   state_process = 0; 
}

void PROCESS_PRODUCING_action(){
  Serial.println("Process producing");
}

void VIN_0_action(){
   Serial.println("VIN estado 0");
   digitalWrite(v_in, LOW);
   valve_in = false;

  outcoming_msg.enqueue(close_vin);
}

void VIN_1_action(){
    Serial.println("VIN estado 1 - Filling");
    digitalWrite(v_in, HIGH);
    valve_in = true;

      
      state_process = 1;
}

void VIN_level_H1_action(){
  Serial.println("Full");
}

void VIN_open_vin_action(){
   outcoming_msg.enqueue(open_vin);
}

void VOUT_0_action(){
    outcoming_msg.enqueue(close_vout);
    Serial.println("VOUT estado 0");
    digitalWrite(v_out, LOW);
    valve_out = false;
    
    
    System.trigger_if_possible(finish);
    
}

void VOUT_1_action(){
  Serial.println("VOUT estado 1 - Draining");
  digitalWrite(v_out, HIGH);
  valve_out = true;
  
  state_process = 4;
}

void VOUT_open_vout_action(){
  outcoming_msg.enqueue(open_vout);
}

void VOUT_level_L1_action(){
   Serial.println("Empty");
}


void MIXER_0_action(){
    Serial.println("MIXER turned off");
    mixer = false;
//    CAN.beginPacket(1);
//    CAN.write(turn_off_mixer);
//    CAN.endPacket();
   outcoming_msg.enqueue(turn_off_mixer);
}

void MIXER_1_action(){
  Serial.println("MIXER turned on");
  mixer = true;
//  CAN.beginPacket(1);
//  CAN.write(turn_on_mixer);
//  CAN.endPacket();
  outcoming_msg.enqueue(turn_on_mixer);
}

void PUMP_0_action(){
    Serial.println("PUMP turned off");
    pump = false;

    outcoming_msg.enqueue(turn_off_pump);
}

void PUMP_1_action(){
  Serial.println("PUMP turned on");
  pump = true;

  outcoming_msg.enqueue(turn_on_pump);
}

void TEMP_0_action(){
    Serial.println("TEMP control turned off");
//    CAN.beginPacket(1);
//    CAN.write(turn_off_tcontrol);
//    CAN.endPacket();
    outcoming_msg.enqueue(turn_off_tcontrol);
    
}

void TEMP_1_action(){
  

  
}

void TEMP_turn_on_tcontrol_action(){
  Serial.println("TEMP control turned on");

    outcoming_msg.enqueue(turn_on_tcontrol);
    state_process = 2;
 
}

void TEMP_heated_action(){
    Serial.println("Temp control heated");
    state_process = 3;
    
}

void TEMP_cooled_action(){
  Serial.println("Temp control cooled");
  
}


void S2_0_action(){
    S2.enable(close_vin);
    Serial.println("S2 estado 0: ");
    
}

void S2_1_action(){
    S2.disable(close_vin);
    Serial.println("S2 estado 1: ");
    
}

void S3_0_action(){
    S3.enable(close_vout);
    Serial.println("S3 estado 0: ");
    
}

void S3_1_action(){
    S3.disable(close_vout);
    Serial.println("S3 estado 1: ");
    //Serial.println(S1.verifica(a2));
}

void S4_0_action(){
  Serial.println("S4 estado 0: ");
  S4.disable(open_vin);
}

void S4_1_action(){
  Serial.println("S4 estado 1: ");
  S4.enable(open_vin);
}

void S4_2_action(){
  Serial.println("S4 estado 2: ");
  S4.enable(open_vin);
}

void S5_0_action(){
  Serial.println("S5 estado 0: ");
  S5.disable(open_vout);
}

void S5_1_action(){
  Serial.println("S5 estado 1: ");
  S5.enable(open_vout);
}

void S5_2_action(){
  Serial.println("S5 estado 2: ");
  S5.enable(open_vout);
}

void S6_0_action(){
  Serial.println("S6 estado 0: ");
  S6.disable(turn_on_mixer);
  
}

void S6_1_action(){
  Serial.println("S6 estado 1: ");
  S6.enable(turn_on_mixer);
  
  
}
void S6_2_action(){
  Serial.println("S6 estado 2: ");
  S6.enable(turn_on_mixer);
}

void S7_0_action(){
  Serial.println("S7 estado 0: ");
  S7.disable(turn_off_mixer);
}

void S7_1_action(){
  Serial.println("S7 estado 1: ");
  S7.enable(turn_off_mixer);

}

void S7_2_action(){
  Serial.println("S7 estado 2: ");
  S7.enable(turn_off_mixer);
}

void S8_0_action(){
  Serial.println("S8 estado 0: ");
  S8.disable(turn_on_pump);
}

void S8_1_action(){
  Serial.println("S8 estado 1: ");
  S8.enable(turn_on_pump);
}

void S8_2_action(){
  Serial.println("S8 estado 2: ");
  S8.disable(turn_on_pump);
}

void S9_0_action(){
  Serial.println("S9 estado 0: ");
  S9.disable(turn_off_pump);
}

void S9_1_action(){
  Serial.println("S9 estado 1: ");
  S9.enable(turn_off_pump);
}

void S9_2_action(){
  Serial.println("S9 estado 2: ");
  S9.disable(turn_off_pump);
}

void S10_0_action(){
  Serial.println("S10 estado 0: ");
  S10.disable(turn_on_tcontrol);
}

void S10_1_action(){
  Serial.println("S10 estado 1: ");
  S10.enable(turn_on_tcontrol);
}

void S10_2_action(){
  Serial.println("S10 estado 2: ");
  S10.enable(turn_on_tcontrol);
}

void S11_0_action(){
  Serial.println("S11 estado 0: ");
  S11.enable(turn_off_tcontrol);
}

void S11_1_action(){
  Serial.println("S11 estado 1: ");
  S11.disable(turn_off_tcontrol);
}

void S11_2_action(){
  Serial.println("S11 estado 2: ");
  S11.disable(turn_off_tcontrol);
}
