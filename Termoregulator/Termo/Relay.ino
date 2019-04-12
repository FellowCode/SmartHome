#define RELEY_PIN   4

void RelayOn(){
    digitalWrite(RELEY_PIN, HIGH);                                                   //Включение реле
    isWork = true;
}

void RelayOff(){
  digitalWrite(RELEY_PIN, LOW);                                                    //Отключение реле
  isWork = false;
}
