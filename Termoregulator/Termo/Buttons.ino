
bool btnLong = false;

/////////*******************/////////////
///BUTTONS
///***********************////

void BtnMode()
{
  //Serial.println("MODE!");
  
  if(nSheet == WiFiSetupSheet)
    nSheet = SETTINGS;
  else if (nSheet == sheetCount)
    nSheet = MAIN;
  else
    nSheet++;
  tftDrawData();
}
void BtnSet()
{
  ///Serial.println("SET!");
  switch(nSheet){
    case MAIN: break;
    case SETTINGS: tftSettingsChangeMode(); break;
    case WiFiSetupSheet: BtnSetWiFiSetup(); break;
  }
}
void BtnUp()
{
  //Serial.println("UP!");
  switch(nSheet){
    case MAIN: needTemp += 0.1f; needTempChanged = true; break;                            //mainSheet
    case SETTINGS: BtnUpSettingsSheet(); break;  //settings Sheet
    case STATISTIC: BtnUpStatistic(); break;
    case WiFiSetupSheet: BtnUpWiFiSetup(); break;
  }
  
}
void BtnDown()
{
  //Serial.println("DOWN!");
  switch(nSheet){
    case MAIN: needTemp -= 0.1f; needTempChanged = true; break;                           //mainSheet
    case SETTINGS: BtnDownSettingsSheet(); break;  //settings Sheet
    case STATISTIC: BtnDownStatistic(); break;
    case WiFiSetupSheet: BtnDownWiFiSetup(); break;
  }
}
void longBtnMode()
{
  //Serial.println("LONG MODE!");
}
void longBtnSet()
{
  //Serial.println("LONG SET!");
  if(!btnLong)
    switch(nSheet){
      case 1: BtnSetSettingsLong(); break;
    }
  btnLong = true;
}
void longBtnUp()
{
  //Serial.println("LONG UP!");
  switch(nSheet){
    case MAIN: needTemp += 0.1f; needTempChanged = true; tftDrawNeedTemp(); break;
    case SETTINGS: BtnUpLongSettingsSheet(); break;
  }
}
void longBtnDown()
{
  //Serial.println("LONG DOWN!");
  switch(nSheet){
    case MAIN: needTemp -= 0.1f; needTempChanged = true; tftDrawNeedTemp(); break;
    case SETTINGS: BtnDownLongSettingsSheet(); break;
  }
}
void BtnReleased()
{
  btnLong = false;
  SaveFloat(A_NEED_TEMP, needTemp);
  if(nSheet == 0)
    checkTemp();
  tftUpdateData();
}

////////////////////////////
////SETTINGS
///////////////////////////

void BtnUpSettingsSheet()
{
  if(!tftChangeMode)
  {
    settingSelect--; 
    tftDrawSettingSelect();
  }
  else
    tftChangeSettingsParam(1);
}
void BtnUpLongSettingsSheet()
{
  if(tftChangeMode)
  {
    if(settingSelect == Watts)      // Watts
      tftChangeSettingsParam(10);
    else
      tftChangeSettingsParam(1);
  }
    
}
void BtnDownSettingsSheet()
{
  if(!tftChangeMode)
  {
    settingSelect++;
    tftDrawSettingSelect();
  }
  else
    tftChangeSettingsParam(-1);
}
void BtnDownLongSettingsSheet()
{
  if(tftChangeMode)
  {
    if(settingSelect == Watts)      // Watts
      tftChangeSettingsParam(-10);
    else
      tftChangeSettingsParam(-1);
  }
}
void BtnSetSettingsLong()
{
  if(settingSelect == ResetWatts){               //reset Watts
    KWt = 0;
    tftSettingsSelectBlink();
  }
}
/////////////////////////////
/////STATISTIC
/////////////////////////////
void BtnUpStatistic()
{
  ChangeStatisticDay(-1);     //daysAgo --
}
void BtnDownStatistic()
{
  ChangeStatisticDay(+1);     //daysAgo ++
}
////////////////////////////
////  WiFi SETUP
////////////////////////////
void BtnUpWiFiSetup()
{
  settingSelect--;
  tftDrawSettingSelect();
}
void BtnDownWiFiSetup()
{
  settingSelect++;
  tftDrawSettingSelect();
}
void BtnSetWiFiSetup()
{
  switch(settingSelect){
    case WiFiHotspot: if(clientIsConnected) WiFiDisconnect(); if(!WiFiOn) CreateAP(); else StopAP(); break;
    case WiFiClient: if(ApIsCreated) StopAP(); if(!WiFiOn) WiFiConnect(); else WiFiDisconnect();break;
  }
  tftDrawWiFiParam(WiFiHotspot);
  tftDrawWiFiParam(WiFiClient);
  tftDrawWiFiParam(SSID);
  tftDrawWiFiParam(Password);
  tftDrawWiFi();
}
