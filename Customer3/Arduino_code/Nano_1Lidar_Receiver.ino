/*Nano_1Lidar_Receiver.ino*/
#include <SoftwareSerial.h>

int dist2; // actual distance measurement of LiDAR
unsigned char check2; // save check value
unsigned char uart2[9]; // save data measured by LiDAR
const int HEADER = 0x59; // frame header of data package
int rec_debug_state2 = 0x01; // receive state for frame
bool triggered2 = false; // track whether a trigger has occurred
bool distanceCalculationActive = true; // flag to control distance calculation

// Define SoftwareSerial pins for LiDAR
SoftwareSerial LidarSerial2(6, 5); // RX, TX for LiDAR

void setup() {
  Serial.begin(115200); // set bit rate of serial port connecting LiDAR with Arduino
  LidarSerial2.begin(115200); // set bit rate of software serial port connecting LiDAR with Arduino

  pinMode(3, OUTPUT); // set pin 3 as output
  digitalWrite(3, HIGH); // ensure pin 3 is high at the start

  while (!Serial);
  Serial.println("LiDAR Distance Measurement");
}

void loop() {
  if (distanceCalculationActive) {
    Get_Lidar2_data();
  }
}

void Get_Lidar2_data() {
  if (LidarSerial2.available()) { // check if serial port has data input
    if (rec_debug_state2 == 0x01) { // the first byte
      uart2[0] = LidarSerial2.read();
      if (uart2[0] == HEADER) {
        check2 = uart2[0];
        rec_debug_state2 = 0x02;
      }
    } else if (rec_debug_state2 == 0x02) { // the second byte
      uart2[1] = LidarSerial2.read();
      if (uart2[1] == HEADER) {
        check2 += uart2[1];
        rec_debug_state2 = 0x03;
      } else {
        rec_debug_state2 = 0x01;
      }
    } else if (rec_debug_state2 == 0x03) {
      uart2[2] = LidarSerial2.read();
      check2 += uart2[2];
      rec_debug_state2 = 0x04;
    } else if (rec_debug_state2 == 0x04) {
      uart2[3] = LidarSerial2.read();
      check2 += uart2[3];
      rec_debug_state2 = 0x05;
    } else if (rec_debug_state2 == 0x05) {
      uart2[4] = LidarSerial2.read();
      check2 += uart2[4];
      rec_debug_state2 = 0x06;
    } else if (rec_debug_state2 == 0x06) {
      uart2[5] = LidarSerial2.read();
      check2 += uart2[5];
      rec_debug_state2 = 0x07;
    } else if (rec_debug_state2 == 0x07) {
      uart2[6] = LidarSerial2.read();
      check2 += uart2[6];
      rec_debug_state2 = 0x08;
    } else if (rec_debug_state2 == 0x08) {
      uart2[7] = LidarSerial2.read();
      check2 += uart2[7];
      rec_debug_state2 = 0x09;
    } else if (rec_debug_state2 == 0x09) {
      uart2[8] = LidarSerial2.read();
      if (uart2[8] == check2) {
        dist2 = uart2[2] + uart2[3] * 256; // the distance
        Serial.print("dist2 = ");
        Serial.println(dist2); // output measured distance value of LiDAR
        
        if (dist2 < 800 && dist2 > 0 && !triggered2) {
          Serial.println("Triggered");
          triggered2 = true;
          distanceCalculationActive = false; // stop distance calculation
          digitalWrite(3, LOW); // turn on pin 3
          delay(10000); // wait for 10 seconds
          digitalWrite(3, HIGH); // turn off pin 3
          distanceCalculationActive = true; // resume distance calculation
        } else if (dist2 >= 50) {
          triggered2 = false;
        }
      }
      rec_debug_state2 = 0x01;
    }
  }
}
