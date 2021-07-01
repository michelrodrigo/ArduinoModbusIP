void PROCESS_0_action(){
   Serial.println("Process: Idle");
   System.trigger_if_possible(close_vout);
   drain_out = false;
   cool = false; 
   state_process = 0;    
}

void PROCESS_1_action(){
   Serial.println("Process: Filling");
   Output = random(10, 255);
   analogWrite(outputPin, Output);  
   System.trigger_if_possible(open_vin);   
   state_process = 1;
}

void PROCESS_2_action(){
   Serial.println("Process: Heating");
   System.trigger_if_possible(close_vin);
   mixer = true;
   state_process = 2;
}

void PROCESS_3_action(){
   Serial.println("Process: Cooling");
   aux = 0;
   pump = true;
   state_process = 3;
}

void PROCESS_4_action(){
   Serial.println("Process: Draining");
   mixer = false;
   System.trigger_if_possible(open_vout);
   Output = 0;
   pump = false;
   state_process = 4;
}


void VIN_0_action(){
   Serial.println("VIN estado 0");
   digitalWrite(v_in, LOW);
   valve_in = false;
}

void VIN_1_action(){
    Serial.println("VIN estado 1");
    digitalWrite(v_in, HIGH);
    valve_in = true;
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

void S1_0_action(){
    S1.enable(open_vin);
    S1.enable(open_vout);
    Serial.println("S1 estado 0: ");
    
}

void S1_1_action(){
    S1.disable(open_vin);
    S1.disable(open_vout);
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
