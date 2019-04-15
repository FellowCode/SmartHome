#define RELEY_PIN   4

void RelaySetup(){
  pinMode(RELEY_PIN, OUTPUT);
}

void RelayOn(){
  digitalWrite(RELEY_PIN, LOW);                                                   //Включение реле
  isWork = true;
}

void RelayOff(){
  digitalWrite(RELEY_PIN, HIGH);                                                    //Отключение реле
  isWork = false;
}
