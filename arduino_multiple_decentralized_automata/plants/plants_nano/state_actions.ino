//void VIN_0_action(){
//   Serial.println("VIN estado 0");
//   digitalWrite(v_in, LOW);
//}
//
//void VIN_1_action(){
//    Serial.println("VIN estado 1");
//    digitalWrite(v_in, HIGH);
//}
//
//void VOUT_0_action(){
//    Serial.println("VOUT estado 0");
//    digitalWrite(v_out, LOW);
//    
//}
//
//void VOUT_1_action(){
//  Serial.println("VOUT estado 1");
//  digitalWrite(v_out, HIGH);
//
//}
//
//void TANK_0_action(){
//  Serial.println("esvaziou");
//
//  CAN.beginPacket(1);
//   CAN.write(level_L1);
//   CAN.endPacket();
// 
//}
//
//void TANK_1_action(){
//    Serial.println("enchendo");
//}
//
//void TANK_2_action(){
//  Serial.println("encheu");
//   
//   CAN.beginPacket(1);
//   CAN.write(level_H1);
//   CAN.endPacket();
//  
//}
//
//void TANK_3_action(){
//    Serial.println("esvaziando");
//}
//
//void MIXER_0_action(){
//    Serial.println("MIXER turned off");
//    
//}
//
//void MIXER_1_action(){
//  Serial.println("MIXER turned on");
// 
//}
//
//void PUMP_0_action(){
//    Serial.println("PUMP turned off");
//    
//}
//
//void PUMP_1_action(){
//  Serial.println("PUMP turned on");
//  
//}
//
//void TEMP_0_action(){
//    Serial.println("TEMP control turned off");
//    Output = 0;
//    
//    
//}
//
//void TEMP_1_action(){
//  Serial.println("TEMP control turned on");
//  aux = 0;
//}
//
//void TEMP_2_action(){
//    Serial.println("Temp control heated");
//    aux = 0;
//    CAN.beginPacket(1);
//    CAN.write(heated);
//    CAN.endPacket();
//}
//
//void TEMP_3_action(){
//  Serial.println("Temp control cooled");
//  CAN.beginPacket(1);
//  CAN.write(cooled);
//  CAN.endPacket();
//  
//}
