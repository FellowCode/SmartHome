
///********************///
///DISPLAY
///********************///

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY     0xD6BA
#define SKY_BLUE 0x865E
#define LEMON    0xFFF4

#define BG_COLOR                 0x0000
#define BG_VALUES_COLOR          0x0000     //default == BG_COLOR
#define TEMP_MAIN_COLOR          0xFFFF
#define TEMP_HEAT_COLOR          0xF800
#define NEED_TEMP_COLOR          0xFFFF
#define TIME_COLOR               0xFFFF
#define MAIN_TEXT_COLOR          0xFFFF
#define HUMIDITY_COLOR           0x865E
#define KWT_COLOR                0xFFF4
#define SETTING_SELECT_COLOR     0xF81F
#define AXIS_COLOR               0xF800
#define GRAPH_COLOR              0xF81F
#define GRAPH_POINT_COLOR        0xFFF4

#define TFT_PARAM_INTERVAL 14                //интервал между элементами списка в пикселях
#define TFT_PARAM_MARGIN   15                //отступ списка от верхнего края экрана в пикселях


extern const unsigned char PROGMEM Termometr [];
extern const unsigned char PROGMEM Steam [];
extern const unsigned char PROGMEM Lightning [];
extern const unsigned char PROGMEM WiFiIcon [];


#define SETTINGS_COUNT  8
const char settings[SETTINGS_COUNT][16] = {"Date", "Time", "Display (sec)", "Hysteresis", "Watts", "Reset watts", "WiFi setup", "Reset params"};

#define WIFI_SETTINGS_COUNT 5
const char WiFiSettings[WIFI_SETTINGS_COUNT][16] = {"Wi-Fi Hotspot", "Wi-Fi Client", "SSID", "Password", "IP"};

//***************//
//MAIN
//***************//
bool currentIsWork = false;
float currentTemp = 0;
float currentNeedTemp = 0;
String currentHumidity = "";
String currentKWt = "";

//***************//
//SETTINGS
//***************//
String currentDate = "";

byte changeLevel = 0;
bool paramVisible = true;

//***************//
//STATISTIC
//***************//
time_t statDate;
byte daysAgo = 0;
byte lastPointX;
byte lastPointY;

///////////////////
//***************//
///////////////////
byte currentSheet = -1;
String currentTime = "";
int currentBattery = 0;
bool currentWiFiOn = false;
bool currentApIsCreated = false;
bool currentClientIsConnected = false;

void tftDrawData()
{
  resetSettingSheetValues();
  if(isActive)
  {
    if(currentSheet != nSheet)
    {
      switch(nSheet){
        case MAIN: tftDrawMain(); break;
        case SETTINGS: tftDrawSettings(); break;
        case STATISTIC: tftDrawStatistic(); break;
      }
      currentSheet = nSheet;
    }
    else
      tftUpdateData();
  }
}
void tftUpdateData()
{
  switch(nSheet){
    case MAIN: tftUpdateMain(); break;
    case SETTINGS: tftUpdateSettings(); break;
    case WiFiSetupSheet: tftUpdateWiFiSettings(); break;
  }
}
void tftDrawMain()
{
  tft.fillScreen(BG_COLOR);
  tftDrawTime();
  tftDrawWiFi();
  tftDrawBattery();
  tftDrawTemp();
  tftDrawNeedTemp();
  tftDrawHumidity();
  tftDrawKWt();
}
void tftUpdateMain()
{
  if(getTime() != currentTime)
    tftDrawTime();
  if(currentWiFiOn != WiFiOn || currentApIsCreated != ApIsCreated || currentClientIsConnected != clientIsConnected)
    tftDrawWiFi();
  if(procBattery != currentBattery)
    tftDrawBattery();
  if(temp != currentTemp || currentIsWork != isWork)
    tftDrawTemp();
  if(needTemp != currentNeedTemp)
    tftDrawNeedTemp();
  if(String(humidity, 1) != currentHumidity)
    tftDrawHumidity();
  if(String(KWt, 1) != currentKWt)
    tftDrawKWt();
}
void tftDrawTemp()
{
  currentTemp = temp;
  currentIsWork = isWork;
  tft.fillRect(0,30, 93, 30, BG_VALUES_COLOR);
  tft.setCursor(0, 30); 
  if(isWork)
  {
    tft.drawBitmap(105, 30, Termometr, 15, 30, TEMP_HEAT_COLOR);
    tft.setTextColor(TEMP_HEAT_COLOR);
  }
  else
  {
    tft.drawBitmap(105, 30, Termometr, 15, 30, TEMP_MAIN_COLOR);
    tft.setTextColor(TEMP_MAIN_COLOR);
  }
  tft.setTextSize(4);
  tft.print(temp, 1);
  tft.setTextSize(2);
  tft.print(char(247));
}
void tftDrawNeedTemp()
{
  currentNeedTemp = needTemp;
  tft.setCursor(0, 62);
  tft.fillRect(0,60,49, 20, BG_VALUES_COLOR);
  tft.setTextSize(2);
  tft.setTextColor(NEED_TEMP_COLOR);
  tft.println(needTemp, 1);
  tft.setCursor(50, 62);
  tft.setTextSize(1);
  tft.print(char(247));
  tft.setTextSize(2);
  tft.print("C");
}
void tftDrawHumidity()
{
  currentHumidity = String(humidity, 1);
  tft.fillRect(23, 85, 46, 16, BG_VALUES_COLOR);
  tft.drawBitmap(0, 82, Steam, 20, 20, HUMIDITY_COLOR);
  tft.setTextColor(HUMIDITY_COLOR);
  tft.setTextSize(2);
  tft.setCursor(23, 85); 
  tft.print(currentHumidity);
  tft.setCursor(73, 85); 
  tft.print("%");
}
void tftDrawKWt()
{
  currentKWt = String(KWt, 1);
  tft.fillRect(10, 110, currentKWt.length()*11+2, 16, BG_VALUES_COLOR);
  tft.drawBitmap(2, 108, Lightning, 5, 16, KWT_COLOR);
  tft.setTextColor(KWT_COLOR);
  tft.setTextSize(2);
  tft.setCursor(10, 110); 
  tft.print(currentKWt);
  tft.setTextSize(1);
  tft.print(" ");
  tft.setTextSize(2);
  tft.print("KWt");
}
///***************************************///
///           SETTINGS SHEET              ///
///***************************************///

void tftDrawSettings()
{
  tft.fillScreen(BG_COLOR);
  currentTime = "";
  tftDrawWiFi();
  tftDrawBattery();
  tft.setCursor(3, 0);
  tft.setTextSize(1);
  tft.setTextColor(MAIN_TEXT_COLOR);
  tft.print("Settings");
  for(int i=0; i < SETTINGS_COUNT; i++)
  {
    tft.setCursor(4, TFT_PARAM_MARGIN + i*TFT_PARAM_INTERVAL);
    tft.print(settings[i]);
    tftDrawSettingsParam(i);
  }
  tftDrawSettingSelect();
}
void resetSettingSheetValues()
{
  changeLevel = 0;
  settingSelect = -1;
  paramVisible = true;
  tftChangeMode = false;
  currentDate = "";
  currentTime = "";
}
void tftUpdateSettings()
{
  if(currentDate != getDate())
    tftDrawSettingsParam(0);
  if(currentTime != getTime())
    tftDrawSettingsParam(1);
  if(currentWiFiOn != WiFiOn || currentApIsCreated != ApIsCreated || currentClientIsConnected != clientIsConnected)
  {
    tftDrawWiFi();
  }
    
}
void tftSettingsChanged()
{
  if(nSheet == SETTINGS)
    for(int i=0; i < SETTINGS_COUNT; i++)
      tftDrawSettingsParam(i);
  else
    tftUpdateData();
}
String getSettingParametr(byte index)
{
  String param = "";
  switch(index){
    case Date: param = getDate(); break;
    case Time: param = getTime(); break;
    case Display: param = String(tftActiveTime); break;
    case Hysteresis: param = String(hysteresis, 1); break;
    case Watts: param = String(watts); break;
  }
  return param;
}
void tftDrawSettingsParam(byte index)
{
  String param = getSettingParametr(index);
  switch(index){
    case 0: currentDate = param; break;
    case 1: currentTime = param;
  }
  tft.fillRect(124 - 6*param.length(), TFT_PARAM_MARGIN+index*14, 6*param.length(), 8, BG_VALUES_COLOR);
  tftTextRightAlign(param, 1, 124, TFT_PARAM_MARGIN+index*TFT_PARAM_INTERVAL);
}
void tftBlinkParam()
{
  paramVisible = !paramVisible;
  switch(settingSelect)
  {
    case 0: tftDrawDateTimePart(); break;                                     //DateTime
    case 1: tftDrawDateTimePart(); break;                                     //DateTime
    default: if(paramVisible) 
               tftDrawSettingsParam(settingSelect); 
             else 
               tft.fillRect(124 - 6*(getSettingParametr(settingSelect).length()+1), TFT_PARAM_MARGIN+settingSelect*14, 6*(getSettingParametr(settingSelect).length()+1), 8, BG_VALUES_COLOR);
  }
}
void tftDrawSettingSelect()                          //Рамка вокруг выбранного элемента
{
  if((settingSelect == SETTINGS_COUNT && nSheet == SETTINGS)||                    //Вернуть выбор в начало при превышении элементов списка для страницы настроек
     (settingSelect == 2 && nSheet == WiFiSetupSheet))        //Вернуть выбор в начало при превышении элементов списка для страницы вай фай настроек
  {
    settingSelect = 0;
    tft.drawRect(0, TFT_PARAM_MARGIN - 3 + (SETTINGS_COUNT-1)*TFT_PARAM_INTERVAL, 128, TFT_PARAM_INTERVAL - 1, BG_VALUES_COLOR);
  }
  if(settingSelect < 0)                                                           //Скрыть рамку выбора при выбранном -1 элементе
  {
    settingSelect = -1;
    tft.drawRect(0, TFT_PARAM_MARGIN - 3, 128, TFT_PARAM_INTERVAL - 1, BG_VALUES_COLOR);
  }
  else {
    if(settingSelect > 0)                                                         //Скрыть рамку сверху выбранного элемента
      tft.drawRect(0, TFT_PARAM_MARGIN - 3 + (settingSelect - 1)*TFT_PARAM_INTERVAL, 128, TFT_PARAM_INTERVAL - 1, BG_VALUES_COLOR);
    if((settingSelect < SETTINGS_COUNT - 1 && nSheet == SETTINGS)||               //Скрыть рамку снизу выбранного элемента (страница настроек)
      (settingSelect < WIFI_SETTINGS_COUNT - 1 && nSheet == WiFiSetupSheet))      //Скрыть рамку снизу выбранного элемента (страница вай фай настроек)               
      tft.drawRect(0, TFT_PARAM_MARGIN - 3 + (settingSelect + 1)*TFT_PARAM_INTERVAL, 128, TFT_PARAM_INTERVAL - 1, BG_VALUES_COLOR);
    tft.drawRect(0, TFT_PARAM_MARGIN - 3 + settingSelect*TFT_PARAM_INTERVAL, 128, TFT_PARAM_INTERVAL - 1, SETTING_SELECT_COLOR);
  }
}
void tftSettingsSelectBlink()
{
  tft.drawRect(0, TFT_PARAM_MARGIN - 3 + settingSelect*TFT_PARAM_INTERVAL, 128, TFT_PARAM_INTERVAL - 1, BG_VALUES_COLOR);
  delay(100);
  tft.drawRect(0, TFT_PARAM_MARGIN - 3 + settingSelect*TFT_PARAM_INTERVAL, 128, TFT_PARAM_INTERVAL - 1, SETTING_SELECT_COLOR);
}
void tftSettingsChangeMode()
{
  if(settingSelect == Date)         //Date
  {
    paramVisible = true;
    tftDrawDateTimePart();
    if(changeLevel < 3)              //next edit
    {
      tftChangeMode = true;
      changeLevel++;
    }
    else                             //end edit
    {
      tftChangeMode = false;
      changeLevel = 0;
      SaveDateTime();
    }
  }
  else if(settingSelect == Time)      //Time
  {
    paramVisible = true;
    tftDrawDateTimePart();
    if(changeLevel < 2)                //next edit
    {
      tftChangeMode = true;
      changeLevel++;
    }
    else                             //end edit
    {
      tftChangeMode = false;
      changeLevel = 0;
      SaveDateTime();
    }
  }
  else if(settingSelect == WiFiSetup)      // WiFi Setup
  {
    nSheet = WiFiSetupSheet;
    resetSettingSheetValues();
    tftDrawWiFiSetup();
  }
  else if(settingSelect != -1 && settingSelect != ResetWatts && settingSelect < SETTINGS_COUNT - 1)      //Params
  {
    tftChangeMode = !tftChangeMode;
    if(!tftChangeMode)
    {
      tftDrawSettingsParam(settingSelect);
      SaveParams();
    }  
  }
}
void tftDrawDateTimePart()
{
  String param = "";
  byte cursorX = 124;
  byte cursorY = TFT_PARAM_MARGIN + settingSelect*14;
  if(settingSelect == 0)
  {
    switch(changeLevel)
    {
      case 1: param = getDay(); cursorX-=6*10; break;
      case 2: param = getMonth(); cursorX-=6*7; break;
      case 3: param = getYear(); cursorX-=6*4; break;
    }
  }
  else
  {
    switch(changeLevel)
    {
      case 1: param = getHour(); cursorX-=6*5; break;
      case 2: param = getMinute(); cursorX-=6*2; break;
    }
  }
  if(paramVisible)
  {
    tft.setCursor(cursorX, cursorY);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.print(param);
  }
  else
    tft.fillRect(cursorX, cursorY, 6*param.length(), 8, BG_VALUES_COLOR);
}
void tftChangeSettingsParam(int mult)
{
  switch(settingSelect){
    case Date: switch(changeLevel){ 
      case 1: setDay(day()+mult); break; 
      case 2: if(month() + mult != 13 && month() + mult != 0) setMonth(month()+mult); break; 
      case 3: setYear(year()+mult);}
      resetStatistic(); 
      break;
    case Time: switch(changeLevel){ 
      case 1: if(hour() + mult != 24 && hour() + mult != -1) setHour(hour()+mult); break; 
      case 2: if(minute() + mult != 60 && minute() + mult != -1) setMinute(minute()+mult); } 
      break;
    case Display: tftActiveTime += 1*mult; tftActiveTime = constrain(tftActiveTime, 10, 1000); break;
    case Hysteresis: hysteresis += 0.1*mult; hysteresis = constrain(hysteresis, 0.1, 50); break;
    case Watts: watts += 1*mult; watts = constrain(watts, 1, 10000); break;
  }
}

///*********************///**************************************////////////////////////////////////
///    STATISTIC        ///**************************************////////////////////////////////////
///*********************///**************************************////////////////////////////////////

void tftDrawStatistic()
{
  resetStatParams();
  tft.fillScreen(BG_COLOR);
  tftDrawTime();
  tftDrawWiFi();
  tftDrawBattery();
  tftDrawCoordinateAxis();
  tftDrawStatisticValue();
  tftDrawChart();
}
void resetStatParams()
{
  daysAgo = 0;
  statDate = now();
}
void tftDrawCoordinateAxis()
{
  tft.drawLine(5,15,5,90,AXIS_COLOR);
  tft.drawLine(5,15,1,20,AXIS_COLOR);
  tft.drawLine(5,15,9,20,AXIS_COLOR);
  tft.drawLine(5,90,123,90,AXIS_COLOR);
  tft.drawLine(118,86,123,90,AXIS_COLOR);
  tft.drawLine(118,94,123,90,AXIS_COLOR);
  tft.setCursor(12,15);
  tft.setTextColor(AXIS_COLOR);
  tft.setTextSize(1);
  tft.print("KWt");
  tft.setCursor(108,96);
  tft.print("day");
  for(int i=0; i<7; i++)
  {
    tft.drawLine((i+1)*14, 89, (i+1)*14, 91, AXIS_COLOR);
    tft.setCursor((i+1)*14, 96);
    tft.print(i+1);
  }
}
void tftDrawChart()                                              //Draw graph
{
  tft.fillRect(10,23,93,66, BG_VALUES_COLOR);
  
  byte maxY = 15;
  byte minY = 85;
  float max = getMax(statistic);
  float min = getMin(statistic);

  for(int i=0;i<7;i++)
  {
    byte X = (i+1)*14;
    float y = (statistic[6-i]-min)/max;
    y = y*(maxY-minY)+minY;
    byte Y = byte(y);
    tft.drawPixel(X, Y, GRAPH_COLOR);
    if(i>0)
      tft.drawLine(lastPointX, lastPointY, X, Y, GRAPH_COLOR);    //graph lines
    if(6-i == daysAgo)
      tft.drawCircle(X,Y,3,GRAPH_POINT_COLOR);                    //Select graph point
    lastPointX = X;
    lastPointY = Y;
  }
}
void tftDrawStatisticValue()                             //KWt for selected day
{
  tft.fillRect(1,110,123,8,BG_VALUES_COLOR);
  tft.setCursor(1, 110);
  tft.setTextColor(MAIN_TEXT_COLOR);
  tft.setTextSize(1);
  tft.print(getDateFromVal(statDate));
  tft.print(" ");
  tft.print(statistic[daysAgo],1);
  tft.print("KWt");
}
void ChangeStatisticDay(int mult)
{
  if(daysAgo + mult < 7 && daysAgo + mult >= 0)
  {
    daysAgo+=mult;
    statDate = setDayFromVal(statDate, day(statDate)-mult);
    tftDrawStatisticValue();
    tftDrawChart();
  }
}

////*****************************////
///WiFiSetup
////****************************/////

void tftDrawWiFiSetup()
{
  currentSheet = nSheet;
  tft.fillScreen(BG_COLOR);
  tftDrawTime();
  tftDrawWiFi();
  tftDrawBattery();
  tft.setTextSize(1);
  tft.setTextColor(MAIN_TEXT_COLOR);
  for(int i=0; i<WIFI_SETTINGS_COUNT; i++)
  {
    tft.setCursor(4, TFT_PARAM_MARGIN + i*TFT_PARAM_INTERVAL);
    tft.print(WiFiSettings[i]);
    tftDrawWiFiParam(i);
  }
}
void tftUpdateWiFiSettings(){
  if (currentWiFiOn != WiFiOn || currentApIsCreated != ApIsCreated || currentClientIsConnected != clientIsConnected){
    currentWiFiOn = WiFiOn;
    currentApIsCreated = ApIsCreated;
    currentClientIsConnected = clientIsConnected;
    tftDrawWiFi();
    for (int i=0; i< WIFI_SETTINGS_COUNT; i++)
      tftDrawWiFiParam(i);
  }
}
void tftDrawWiFiParam(int index)
{
  String param = getWiFiParam(index);
  tft.fillRect(124 - 6*(param.length()+1), TFT_PARAM_MARGIN+index*14, 6*(param.length()+1), 8, BG_VALUES_COLOR);
  tftTextRightAlign(param, 1, 124, TFT_PARAM_MARGIN+index*TFT_PARAM_INTERVAL);
}
String getWiFiParam(int index)
{
  String param = "";
  switch(index){
    case WiFiHotspot: if(ApIsCreated) param = "On"; else param = "Off"; break;
    case WiFiClient: if(WiFiOn && !ApIsCreated) if(clientIsConnected) param = "On"; else param = "..."; else param = "Off"; break;
    case SSID: if(ApIsCreated) param = ap_ssid; else if(clientOn) param = clientSSID; else param = "              "; break;
    case Password: if(ApIsCreated) param = ap_password; else if(clientOn) param = clientPassword; else param = "          "; break;
    case IP: if(ApIsCreated) param = IpAddress2String(apIP); else if(clientIsConnected) param = IpAddress2String(localIP); else param = "                "; break;
  }
  return param;
}

////*****************************////
////       MODULES               ////
////*****************************////
void tftTextRightAlign(String text, byte textSize, byte x, byte y)
{
  tft.setTextSize(textSize);
  byte pxSym;
  switch(textSize){
    case 1: pxSym = 6; break;
    case 2: pxSym = 10; break;
  }
  tft.setCursor(x-pxSym*text.length(), y);
  tft.print(text);
}
void tftDrawTime()
{
  if(isActive)
  {
    String time_s = getTime();
    tft.setCursor(0,0);
    tft.fillRect(0,0,30, 8, BG_VALUES_COLOR);
    tft.setTextColor(TIME_COLOR);
    tft.setTextSize(1);
    tft.print(time_s);
    currentTime = time_s;
  }
}
void tftDrawBattery()
{
  currentBattery = procBattery;
  String proc = String(procBattery) + "%";
  tft.fillRect(104,0,18, 8, BG_VALUES_COLOR);
  if(procBattery < 15)
    tft.setTextColor(ST7735_RED);
  else if(procBattery < 70)
    tft.setTextColor(ST7735_YELLOW);
  else
    tft.setTextColor(ST7735_GREEN);
  tftTextRightAlign(proc, 1, 128, 0);
}
void tftDrawWiFi()
{
  currentWiFiOn = WiFiOn;
  currentApIsCreated = ApIsCreated;
  currentClientIsConnected = clientIsConnected;
  if(!WiFiOn)
    tft.fillRect(90,0,10,8, BG_VALUES_COLOR);
  else if(ApIsCreated || clientIsConnected)
    tft.drawBitmap(90, 0, WiFiIcon, 10, 8, ST7735_GREEN);
  else 
    tft.drawBitmap(90, 0, WiFiIcon, 10, 8, ST7735_RED);
}
