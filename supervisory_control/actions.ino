void VIN_0_action(){
   Serial.println("VIN estado 0");
   supervisor_trigger(close_vin);
}

void VIN_1_action(){
    Serial.println("VIN estado 1");
    supervisor_trigger(open_vin);
}

void VOUT_0_action(){
    Serial.println("VOUT estado 0");
    supervisor_trigger(close_vout);
}

void VOUT_1_action(){
  Serial.println("VOUT estado 1");
  supervisor_trigger(open_vout);
}

void TANK_0_action(){
  Serial.println("esvaziou");
  supervisor_trigger(level_L1);
}

void TANK_1_action(){
    Serial.println("enchendo");
    supervisor_trigger(open_vin);
}

void TANK_2_action(){
  Serial.println("encheu");
  supervisor_trigger(level_H1);
}

void TANK_3_action(){
    Serial.println("esvaziando");
    supervisor_trigger(open_vout);
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
