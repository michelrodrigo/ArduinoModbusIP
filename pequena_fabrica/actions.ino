void M1_1_action(){
  L1M1_on();
  L2M1_off();
}

void M1_2_action(){
  L1M1_off();
  L2M1_on();
}

void M1_0_action(){
  L2M1_off();
  L1M1_off();
  G1.trigger(b1);
}

void M2_1_action(){
  L1M2_on();
  L2M2_off();
}

void M2_2_action(){
  L1M2_off();
  L2M2_on();
}

void M2_0_action(){
  L2M2_off();
  L1M2_off();
  G2.trigger(b2);
}

void M3_1_action(){
  L1M3_on();
  L2M3_off();
}

void M3_2_action(){
  L1M3_off();
  L2M3_on();
}

void M3_0_action(){
  L2M3_off();
  L1M3_off();
  G3.trigger(b3);
}

void G1_0_action(){
  Serial.println("G1 estado 0");
  S1.trigger(b1);
}

void G1_1_action(){
  Serial.println("G1 estado 1");
  M1.trigger(1);
  S1.trigger(a1);
}

void G2_0_action(){
  Serial.println("G2 estado 0");
  S2.trigger(b2);
}

void G2_1_action(){
  Serial.println("G2 estado 1");
  S1.trigger(a2);
  S2.trigger(a2);
  M2.trigger(1);
  Serial.println(S1.verifica(a2));
  
  
}

void G3_0_action(){
  Serial.println("G3 estado 0");
  
}

void G3_1_action(){
  Serial.println("G3 estado 1");
  M3.trigger(1);
  S2.trigger(a3);
}

void S1_0_action(){
    S1.desabilita(a2);
    S1.habilita(a1);
    Serial.println("S1 estado 0: ");
    Serial.println(S1.verifica(a1));
    Serial.println(S1.verifica(a2));
    
}

void S1_1_action(){
    S1.desabilita(a1);
    S1.habilita(a2);
    Serial.println("S1 estado 1: ");
    Serial.println(S1.verifica(a2));
}

void S2_0_action(){
    S2.desabilita(a3);
    S2.habilita(a2);
    Serial.println("S2 estado 0: ");
    Serial.println(S2.verifica(a2));
}

void S2_1_action(){
    S2.desabilita(a2);
    S2.habilita(a3);
    Serial.println("S2 estado 1: ");
    //Serial.println(desabilitacao);
}


