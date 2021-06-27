void VIN_0_action(){
   Serial.println("VIN estado 0");
   S1.trigger(close_vin);
}

void VIN_1_action(){
    Serial.println("VIN estado 1");
    S1.trigger(open_vin);
}

void VOUT_0_action(){
    Serial.println("VOUT estado 0");
    S1.trigger(close_vout);
}

void VOUT_1_action(){
  Serial.println("VOUT estado 1");
  S1.trigger(open_vout);
}

void TANK_0_action(){
  Serial.println("esvaziou");
  S1.trigger(level_L1);
}

void TANK_1_action(){
    Serial.println("enchendo");
    S1.trigger(open_vin);
}

void TANK_2_action(){
  Serial.println("encheu");
  S1.trigger(level_H1);
}

void TANK_3_action(){
    Serial.println("esvaziando");
    S1.trigger(open_vout);
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
