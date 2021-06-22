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

void HI_LEVEL_0_action(){
  Serial.println("encheu");
  S1.trigger(full);
}

void LO_LEVEL_0_action(){
    Serial.println("esvaziou");
    S1.trigger(empty);
}


void S1_0_action(){
    S1.desabilita(open_vout);
    S1.habilita(open_vin);
    Serial.println("S1 estado 0: ");
    
    
}

void S1_1_action(){
    S1.desabilita(open_vout);
    S1.habilita(close_vin);
    Serial.println("S1 estado 1: ");
    //Serial.println(S1.verifica(a2));
}

void S1_2_action(){
    S1.desabilita(open_vin);
    S1.habilita(open_vout);
    Serial.println("S1 estado 2: ");
    Serial.println(S1.verifica(close_vout));
}

void S1_3_action(){
    S1.desabilita(open_vin);
    S1.habilita(close_vout);
    Serial.println("S1 estado 3: ");
    //Serial.println(desabilitacao);
}
