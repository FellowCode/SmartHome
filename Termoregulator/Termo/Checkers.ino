///**************************////////
///CHECKERS
///**************************///////

#define SETTING_PARAM_BLINK_DELAY    100        //millis
#define BTN_CHECKER                  10          //cycles
#define BTN_DELAY 400           //Время удержания кнопки (мсек)
#define BTN_LONG_DELAY 50       //Период вызова функции удержания (мсек)

#define DHT_TIMEOUT 10  //период считывания данных dht (сек)

#define WORK_COUNTER_DELAY  10       //Период обновления киловатт (сек)

#define TFT_UPDATE_DELAY    1

#define BAT_DELAY  60    //период считывания заряда батареи (сек)

#define DAY_DELAY  60    //период проверки текущего дня (сек)

unsigned long settingBlinkTimer = 0;
byte btnChecker = 0;
int btnCheckerSum = 0;
long releTimer;
long activeTimer;

bool btn = false;
unsigned long btnTimer = 0;
unsigned long batTimer = 0;
unsigned long tftUpdateTimer = 0;
unsigned long dayTimer = 0;

void checkTftUpdate()
{
  if(tftUpdateTimer < millis())
  {
    tftUpdateData();
    tftUpdateTimer = millis() + TFT_UPDATE_DELAY*1000;
  }
}

void checkSettingBlink()                  //Blink changed setting parametr
{
  if(tftChangeMode && settingBlinkTimer < millis())
  {
    tftBlinkParam();
    settingBlinkTimer = millis() + SETTING_PARAM_BLINK_DELAY;
  }
}

void checkActive()                                              //Проверка активного времени экрана
{
  if(activeTimer<millis())
  {
    isActive = false;
    digitalWrite(TFT_LED, LOW);                                 //Погасить экран
    nSheet = 0;
    resetSettingSheetValues();
  }
  else if(!isActive)
  {
    isActive = true;
    tftDrawData();
    digitalWrite(TFT_LED, HIGH);                                 //Зажечь экран
  }
  
}
void checkTemp()
{
  if (needTempChanged)
    changeWebTargetTemp = true;
  if(!isWork && ((needTemp - hysteresis > temp && !needTempChanged)||(needTemp > temp && needTempChanged)))     //Гистерезис применяется только если необходимая температура не была изменена
  {            
    digitalWrite(RELE_PIN, HIGH);                                                   //Включение реле
    isWork = true;
    releTimer = millis() + WORK_COUNTER_DELAY*1000;
  }
  else if(isWork && needTemp <= temp)
  {
    digitalWrite(RELE_PIN, LOW);                                                    //Отключение реле
    isWork = false;
    //powerTime += getSecond(WORK_COUNTER_DELAY*1000 - (releTimer - millis()));
    //SaveLong(A_POWER_TIME, powerTime);
    float kwt = getKWtH(getSecond(WORK_COUNTER_DELAY*1000 - (releTimer - millis())));
    KWt += kwt;
    SaveFloat(A_KWT, KWt);
    statistic[0] += kwt;
  }
  needTempChanged = false;
}
void checkKWt()
{
  if(isWork && releTimer < millis())
  {
    releTimer += WORK_COUNTER_DELAY*1000;
    //powerTime += WORK_COUNTER_DELAY;
    float kwt = getKWtH(WORK_COUNTER_DELAY);
    KWt += kwt;
    statistic[0] += kwt;
  }
}
void checkBtn()
{
  int param = analogRead(BTN_PIN);
  if(param > 750)
  {
    activeTimer = millis() + tftActiveTime * 1000;
    if(isActive)
    {
      if(!btn)
      {
        if(btnChecker < BTN_CHECKER){
          btnChecker++;
          btnCheckerSum += param;
        }
        else
        {
          //Какая кнопка нажата определяется по среднему арифметическому
          param = btnCheckerSum / btnChecker;
          btnTimer = millis() + BTN_DELAY;
          btn = true;
          if(param < 890)
            BtnMode();
          else if(param < 930)
            BtnSet();
          else if(param < 1000)
            BtnUp();
          else
            BtnDown();
        }
      }
      if(btn && btnTimer < millis())
      {
        if(param < 890)
          longBtnMode();
        else if(param < 930)
          longBtnSet();
        else if(param < 1000)
          longBtnUp();
        else
          longBtnDown();
        btnTimer += BTN_LONG_DELAY;
      }
    }
  }
  else
  {
    if(btn)
      BtnReleased();
    btnChecker = 0;
    btnCheckerSum = 0;
    btn = false;
  }
}
void checkBattery()
{
  int param = analogRead(BAT_PIN);
  if(batTimer < millis() && param < 750)
  {
    float Vbat = (param * 3.3) / 1082;
    float del = 0.453; // R2/(R1+R2)  0.973кОм / (1.175кОм + 0.973кОм)
    float Vin = Vbat / del;
    // уровень заряда в процентах
    procBattery = ((Vin - min_v) / (max_v - min_v)) * 100;
    batTimer = millis() + BAT_DELAY*1000;
  }
}
void dhtGetData()
{
  if(dht_timer < millis())
  {
    float new_humidity = dht.readHumidity(); //Измеряем влажность
    
    float new_temp = dht.readTemperature()*TEMP_CALIBRATION; //Измеряем температуру
    if (!isnan(new_humidity))
      humidity = new_humidity;
    if (!isnan(new_temp)){
      temp = new_temp;
      dht_timer = millis() + DHT_TIMEOUT*1000;
    }
    else
      dht_timer = millis() + 2000;
    checkTemp();
  }
}
void checkWiFi()
{
  checkRequest();
  WiFiClientLoop();
}
void checkDay()
{
  if(currentDay != day())
  {
    for(int i=6; i>=0; i--)
      statistic[i+1] = statistic[i];
    statistic[0] = 0;
  }
}
