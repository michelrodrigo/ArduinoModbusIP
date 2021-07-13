
// Libraries ----------------------------------------------------------
#include "SCT.h"
#include <CAN.h>

// Pins --------------------------------------------------------------
int v_in = 2; //input valve


// Input valve states
State VIN_0(&VIN_0_action, NULL, 0);
State VIN_1(&VIN_1_action, NULL, 1);

Automaton VIN(&VIN_0);

int open_vin = 1;
int close_vin = 3;

char received_event;

void VIN_0_action(){
   Serial.println("VIN estado 0");
   digitalWrite(v_in, LOW);
}

void VIN_1_action(){
    Serial.println("VIN estado 1");
    digitalWrite(v_in, HIGH);
}
void setup() {
  Serial.begin(9600);

  Serial.println("VIN");
  VIN.add_transition(&VIN_0, &VIN_1, open_vin, NULL);
  VIN.add_transition(&VIN_1, &VIN_0, close_vin, NULL);

  pinMode(v_in, OUTPUT);
  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {

    // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (CAN.available()) {
         received_event = (char)CAN.read();
      Serial.print(received_event);
      }
      Serial.println();
    }

    Serial.println();
  }
 
    if(received_event == 'a'){
      VIN.trigger(open_vin);
    }
    else if(received_event == 'b'){
      VIN.trigger(close_vin);
    }
    
  }
