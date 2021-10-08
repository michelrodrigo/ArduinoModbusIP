




void MIXER_0_action(){
    Serial.println("MIXER turned off");
    digitalWrite(mixer, LOW);
}

void MIXER_1_action(){
  Serial.println("MIXER turned on");
 digitalWrite(mixer, HIGH);
}

void PUMP_0_action(){
    Serial.println("PUMP turned off");
    digitalWrite(pump, LOW);
}

void PUMP_1_action(){
  Serial.println("PUMP turned on");
  digitalWrite(pump, HIGH);
}

void TEMP_0_action(){
    Serial.println("TEMP control turned off");
    Output = 0;
    already_heated = false;
    already_cooled = false;
    
    
}

void TEMP_1_action(){
  Serial.println("TEMP control turned on");
  aux = 0;
}

void TEMP_heated_action(){
    Serial.println("Temp control heated");
    aux = 0;
    CAN.beginPacket(1);
    CAN.write(heated);
    CAN.endPacket();
    already_heated = true;
}

void TEMP_cooled_action(){
  Serial.println("Temp control cooled");
  CAN.beginPacket(1);
  CAN.write(cooled);
  CAN.endPacket();
  already_cooled = true;
  
}
