#include "MegaServo.h"

MegaServo megaServo;

void setup()
{
  Serial.begin(115200);
    
  megaServo.attach(7, 8, 5, A4);

  megaServo.write(90);
}

void loop()
{
  if (Serial.available())
  {
    int angle = Serial.parseInt();
    
    Serial.println(angle);
    
    megaServo.write(angle);
  }
}
