# Arduino Modbus IP

This project is the implementation of a hybrid system and its integration with a SCADA system.

The main screen of the SCADA interface is shown below.
![main_screen](https://user-images.githubusercontent.com/12836843/122487626-13e78380-cfb2-11eb-9f74-1fe73cbb9f6a.JPG)

The system is composed of a tank which receives three different types of products. They are mixed and heated for a given time and then cooled. After that, the batch is ready and the tank is drained.

## Arduino codes

The arduino board is the interface between the real sytem and the SCADA. In the [first implemented architecture](https://github.com/michelrodrigo/ArduinoModbusIP/blob/main/arduino_centralized_ethernet.ino), the arduino board implements the controller and the gateway between the real world and the SCADA.

