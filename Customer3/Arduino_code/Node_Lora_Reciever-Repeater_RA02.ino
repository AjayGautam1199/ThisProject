/*Node_Lora_Reciever-Repeater_RA02.ino*/
#include <LoRa.h>

#define SS 15
#define RST 16
#define DIO0 2
#define RELAY_PIN 0  // Define the pin connected to the relay
#define PIN_2 3//2      // Define the pin to monitor

unsigned long relayStartTime = 0;
bool relayOn = false;
String transmitData = ""; // Variable to hold the data to transmit

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Receiver Host");

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    while (1);
  }

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay off initially (assuming active low relay)
  pinMode(PIN_2, INPUT);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  String receivedData = "";

  // If the relay is on, check if it should be turned off
  if (relayOn) {
    unsigned long currentTime = millis();
    if (digitalRead(PIN_2) == LOW || (currentTime - relayStartTime) >= 60000) {
      digitalWrite(RELAY_PIN, HIGH); // Turn off relay
      Serial.println("Relay OFF");
      relayOn = false;

      // Transmit the corresponding data (12 or 25) for 10 seconds
      unsigned long transmitStartTime = millis();
      while (millis() - transmitStartTime < 10000) {
        Serial.print("Transmitting: ");
        Serial.println(transmitData);
        LoRa.beginPacket();
        LoRa.print(transmitData);
        LoRa.endPacket();
        delay(1000); // Send every 1 second
      }

      Serial.println("Transmission complete. Resuming normal operation.");
    }
  }

  // Normal packet receiving
  if (!relayOn && packetSize) {
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }

    Serial.print("Receiving Data: ");
    Serial.println(receivedData);

    if (receivedData == "11" || receivedData == "24") {
      // Set the transmit data based on the received packet
      if (receivedData == "11") {
        transmitData = "12";
      } else if (receivedData == "24") {
        transmitData = "25                                                                                                ";
      }

      // Generate random delay between 5-10 seconds (5000-10000 ms)
      int delayTime = random(5000, 10001);
      Serial.print("Delaying for ");
      Serial.print(delayTime / 1000);
      Serial.println(" seconds");

      delay(delayTime);

      // Turn on relay and record the start time
      digitalWrite(RELAY_PIN, LOW); // Turn on relay (assuming active low relay)
      Serial.println("Relay ON");
      relayStartTime = millis();
      relayOn = true;
    }
  }
}
