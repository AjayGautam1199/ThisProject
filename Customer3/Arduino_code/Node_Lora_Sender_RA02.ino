/*Node_Lora_Sender_RA02.ino*/
#include <LoRa.h>
#define SS 15
#define RST 16
#define DIO0 2
#define TRIGGER_PIN 0

String data = "11"; // Transmitting character "11. Change this for each device in the chain"

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Sender Host");

  // Initialize pin 3 as input
  pinMode(TRIGGER_PIN, INPUT);

  // Initialize LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    delay(100);
    while (1);
  }
}

void loop() {
  int pinState = digitalRead(TRIGGER_PIN);

  if (pinState == LOW) {
    Serial.print("Sending Data: ");
    Serial.println(data);
    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();
    delay(1000); // Continue transmission every 1 second
  } else {
    Serial.println("Pin 3 is not low. Stopping transmission.");
    // No transmission if pin 3 is not low
    delay(1000); // Keep checking every 1 second
  }
}
