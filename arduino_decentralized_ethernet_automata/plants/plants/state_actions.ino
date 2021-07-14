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
