void VIN_0_action(){
   Serial.println("VIN estado 0");
   digitalWrite(v_in, LOW);
}

void VIN_1_action(){
    Serial.println("VIN estado 1");
    digitalWrite(v_in, HIGH);
}

void VOUT_0_action(){
    Serial.println("VOUT estado 0");
    digitalWrite(v_out, LOW);
    
}

void VOUT_1_action(){
  Serial.println("VOUT estado 1");
  digitalWrite(v_out, HIGH);

}

void MIXER_0_action(){
    Serial.println("MIXER turned off");
    
}

void MIXER_1_action(){
  Serial.println("MIXER turned on");
 
}

void PUMP_0_action(){
    Serial.println("PUMP turned off");
    
}

void PUMP_1_action(){
  Serial.println("PUMP turned on");
  
}
