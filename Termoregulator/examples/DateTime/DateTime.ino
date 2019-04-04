#include <TimeLib.h>
#include <EEPROM.h>

void setup()
{
  //EEPROM.begin(512);
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(16, INPUT);
  pinMode(A0, INPUT);
  digitalWrite(5, HIGH);
  Serial.println("ON");
  /*byte seconds = EEPROM.read(0);
  setTime(hour(), minute(), seconds, 28, 2, 2018);
  
  //Serial.println(second());        // returns the second for the given time t
  EEPROM.write(0, byte(second()));
  EEPROM.commit();*/
  
  
  
}

void loop()
{
  Serial.println(analogRead(A0));
  delay(200);
  if (millis() > 5000)
  {
    digitalWrite(5, LOW);
    Serial.println("OFF");
    ESP.deepSleep(5e6);
  }
}

