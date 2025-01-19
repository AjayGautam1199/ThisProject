/*Nano_2Lidar_Transmitter.ino*/
#include <SoftwareSerial.h>

int dist1, dist2; // actual distance measurements of LiDAR
unsigned char check1, check2; // save check value
unsigned char uart1[9], uart2[9]; // save data measured by LiDAR
const int HEADER = 0x59; // frame header of data package
int rec_debug_state1 = 0x01, rec_debug_state2 = 0x01; // receive state for frame
bool triggered1 = false, triggered2 = false; // track whether a trigger has occurred
bool distanceCalculationActive = true; // flag to control distance calculation

// Define SoftwareSerial pins for LiDAR2
SoftwareSerial LidarSerial2(6, 5); // RX, TX for LiDAR2

void setup() {
  Serial.begin(115200); // set bit rate of serial port connecting LiDAR1 with Arduino
  LidarSerial2.begin(115200); // set bit rate of software serial port connecting LiDAR2 with Arduino

  pinMode(3, OUTPUT); // set pin 3 as output
  digitalWrite(3, HIGH); // ensure pin 3 is low at the start

  while (!Serial);
  Serial.println("LiDAR Distance Measurement");
}

void loop() {
  if (distanceCalculationActive) {
    Get_Lidar1_data(); // if this channel is not used, comment out this line
    Get_Lidar2_data(); // if this channel is not used, comment out this line
  }
}

void Get_Lidar1_data() {
  if (Serial.available()) { // check if serial port has data input
    if (rec_debug_state1 == 0x01) { // the first byte
      uart1[0] = Serial.read();
      if (uart1[0] == HEADER) {
        check1 = uart1[0];
        rec_debug_state1 = 0x02;
      }
    } else if (rec_debug_state1 == 0x02) { // the second byte
      uart1[1] = Serial.read();
      if (uart1[1] == HEADER) {
        check1 += uart1[1];
        rec_debug_state1 = 0x03;
      } else {
        rec_debug_state1 = 0x01;
      }
    } else if (rec_debug_state1 == 0x03) {
      uart1[2] = Serial.read();
      check1 += uart1[2];
      rec_debug_state1 = 0x04;
    } else if (rec_debug_state1 == 0x04) {
      uart1[3] = Serial.read();
      check1 += uart1[3];
      rec_debug_state1 = 0x05;
    } else if (rec_debug_state1 == 0x05) {
      uart1[4] = Serial.read();
      check1 += uart1[4];
      rec_debug_state1 = 0x06;
    } else if (rec_debug_state1 == 0x06) {
      uart1[5] = Serial.read();
      check1 += uart1[5];
      rec_debug_state1 = 0x07;
    } else if (rec_debug_state1 == 0x07) {
      uart1[6] = Serial.read();
      check1 += uart1[6];
      rec_debug_state1 = 0x08;
    } else if (rec_debug_state1 == 0x08) {
      uart1[7] = Serial.read();
      check1 += uart1[7];
      rec_debug_state1 = 0x09;
    } else if (rec_debug_state1 == 0x09) {
      uart1[8] = Serial.read();
      if (uart1[8] == check1) {
        dist1 = uart1[2] + uart1[3] * 256; // the distance
        Serial.print("dist1 = ");
        Serial.println(dist1); // output measure distance value of LiDAR
        
        if (dist1 >= 30 && dist1 <= 800 && !triggered1) {
        Serial.println("Triggered: left to right");
        triggered1 = true;
        distanceCalculationActive = false; // stop distance calculation
        digitalWrite(3, LOW); // turn on pin 3
        delay(10000); // wait for 20 seconds
        digitalWrite(3, HIGH); // turn off pin 3
        Serial.println("Stopping transmission; waiting for train to pass");
        delay(120000);
        distanceCalculationActive = true; // resume distance calculation
        } else if (dist1 > 800 || dist1 < 30) {
        triggered1 = false;
        }
      }
      rec_debug_state1 = 0x01;
    }
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
        Serial.println(dist2); // output measure distance value of LiDAR
        
        if (dist2 >= 30 && dist2 <= 100 && !triggered2) {
        Serial.println("Triggered: right to left");
        delay(240000); // pause for 4 minutes
        triggered2 = true;
        } else if (dist2 > 800 || dist2 < 30) {
        triggered2 = false;
        }

      }
      rec_debug_state2 = 0x01;
    }
  }
}
