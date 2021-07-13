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


void VIN_0_action(){
   Serial.println("VIN estado 0");
   digitalWrite(v_in, LOW);
   valve_in = false;
   CAN.beginPacket(0x12);
    CAN.write('b');
    CAN.endPacket();
}

void VIN_1_action(){
    Serial.println("VIN estado 1");
    digitalWrite(v_in, HIGH);
    valve_in = true;
    CAN.beginPacket(0x12);
    CAN.write('a');
    CAN.endPacket();
}

void VOUT_0_action(){
    Serial.println("VOUT estado 0");
    digitalWrite(v_out, LOW);
    valve_out = false;
    
}

void VOUT_1_action(){
  Serial.println("VOUT estado 1");
  digitalWrite(v_out, HIGH);
  valve_out = true;
}

void TANK_0_action(){
  Serial.println("esvaziou");
 
}

void TANK_1_action(){
    Serial.println("enchendo");
}

void TANK_2_action(){
  Serial.println("encheu");
  
}

void TANK_3_action(){
    Serial.println("esvaziando");
}

void MIXER_0_action(){
    Serial.println("MIXER turned off");
    mixer = false;
}

void MIXER_1_action(){
  Serial.println("MIXER turned on");
  mixer = true;
}

void PUMP_0_action(){
    Serial.println("PUMP turned off");
    pump = false;
}

void PUMP_1_action(){
  Serial.println("PUMP turned on");
  pump = true;
}

void TEMP_0_action(){
    Serial.println("TEMP control turned off");
    
}

void TEMP_1_action(){
  Serial.println("TEMP control turned on");
 
}

void TEMP_2_action(){
    Serial.println("Temp control heated");
    
}

void TEMP_3_action(){
  Serial.println("Temp control cooled");
  
}

void S1_0_action(){
    S1.enable(start_process);
    Serial.println("S1 estado 0: ");
    
}

void S1_1_action(){
    S1.disable(start_process);
    
    Serial.println("S1 estado 1: ");
    //Serial.println(S1.verifica(a2));
}

void S1_2_action(){
    S1.enable(start_process);
    
    Serial.println("S1 estado 1: ");
    //Serial.println(S1.verifica(a2));
}

void S2_0_action(){
    S2.enable(close_vin);
    Serial.println("S2 estado 0: ");
    
}

void S2_1_action(){
    S2.disable(close_vin);
    Serial.println("S2 estado 1: ");
    //Serial.println(S1.verifica(a2));
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

void S5_0_action(){
  Serial.println("S5 estado 0: ");
  S5.disable(open_vout);
}

void S5_1_action(){
  Serial.println("S5 estado 1: ");
  S5.enable(open_vout);
}

void S6_0_action(){
  Serial.println("S6 estado 0: ");
  S6.disable(turn_on_mixer);
  S6.disable(turn_off_mixer);
}

void S6_1_action(){
  Serial.println("S6 estado 1: ");
  S6.enable(turn_on_mixer);
  S6.enable(turn_off_mixer);
  
}
void S6_2_action(){
  Serial.println("S6 estado 2: ");
  S6.disable(turn_off_mixer);
}

void S7_0_action(){
  Serial.println("S7 estado 0: ");
  S7.disable(turn_on_pump);
  S7.disable(turn_off_pump);
}

void S7_1_action(){
  Serial.println("S7 estado 1: ");
  S7.enable(turn_on_pump);
  S7.enable(turn_off_pump);
}

void S7_2_action(){
  Serial.println("S7 estado 2: ");
  S7.disable(turn_off_pump);
}

void S8_0_action(){
  Serial.println("S8 estado 0: ");
  S8.disable(turn_on_tcontrol);
}

void S8_1_action(){
  Serial.println("S7 estado 1: ");
  S8.enable(turn_on_tcontrol);
}
