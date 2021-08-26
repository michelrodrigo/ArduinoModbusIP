void VIN_0_action(){
   Serial.println("VIN estado 0");
   digitalWrite(v_in, LOW);
}

void VIN_1_action(){
    digitalWrite(v_in, HIGH);
}

void VIN_level_H1_action(){
   Serial.println("Full");
   CAN.beginPacket(1);
   CAN.write(level_H1);
   CAN.endPacket();
   
}

void VIN_open_vin_action(){
  Serial.println("VIN estado 1 - Enchendo");
 
}


void VOUT_0_action(){
    Serial.println("VOUT estado 0");
    digitalWrite(v_out, LOW);
    
}

void VOUT_1_action(){
 
  digitalWrite(v_out, HIGH);

}

void VOUT_level_L1_action(){
   Serial.println("Empty");
   CAN.beginPacket(1);
   CAN.write(level_L1);
   CAN.endPacket();
}

void VOUT_open_vout_action(){
   Serial.println("VOUT estado 1 - Esvaziando");
}
