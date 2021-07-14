
#include "SCT.h"
#include <CAN.h>

// Events ---------------------------------------------------------------
int controllable_events[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23};
int uncontrollable_events[] = {2, 4, 6, 8, 10, 12, 14};
#define open_vin        controllable_events[0]
#define close_vin       controllable_events[1]
#define open_vout       controllable_events[2]
#define close_vout      controllable_events[3]
#define init            controllable_events[4]
#define turn_on_mixer   controllable_events[5]
#define turn_off_mixer  controllable_events[6]
#define turn_on_pump    controllable_events[7]
#define turn_off_pump   controllable_events[8]
#define turn_on_tcontrol  controllable_events[9]
#define turn_off_tcontrol controllable_events[10]

#define level_H1        uncontrollable_events[0]
#define level_L1        uncontrollable_events[1]
#define full            uncontrollable_events[2]
#define heated          uncontrollable_events[3]
#define cooled          uncontrollable_events[4]
#define empty           uncontrollable_events[5]
#define process_start   uncontrollable_events[6]

#define NUM_C_EVENTS 12
#define NUM_U_EVENTS 7


void setup() {
  Serial.begin(9600);
  CAN.setPins(53);
  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  
  
}

void loop() {

  
  if (Serial.available()) {
    int input = Serial.parseInt();

    CAN.beginPacket(1);
    CAN.write(input);
    CAN.endPacket();
  }
  

}
