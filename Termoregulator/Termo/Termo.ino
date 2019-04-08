#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define LOOP_DELAY  5
unsigned long loopTimer;
//////*******************///////////

#define DHTPIN 12
#define TEMP_CALIBRATION   0.88

DHT dht(DHTPIN, DHT22); //Инициация датчика

float temp, humidity;
long dht_timer = 0;

//////***DISPLAY****/////////

#define TFT_CS     0
#define TFT_DC     2
#define TFT_LED    15

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC);

enum {MAIN, SETTINGS, STATISTIC};    //Sheets
enum {Date, Time, Display, Hysteresis, Watts, ResetWatts, WiFiSetup, ResetParams};
enum {WiFiHotspot, WiFiClient, SSID, Password, IP};

const byte sheetCount = 3;
bool isActive = false;
byte nSheet = 0;
const byte WiFiSetupSheet = 10;

/////*****BUTTONS*****////////

#define BTN_PIN     A0

////*****BATTERY*****//////
#define BAT_PIN    A0

float min_v = 3.1, max_v = 4.2;
int procBattery;

////***Wi-Fi***////////
const char* ap_ssid = "TermoR00v01";//"TermoR00v01";
const char* ap_password = "12345678";
String clientSSID = "";
String clientPassword = "";

bool WiFiOn = false;
bool ApIsCreated = false;
bool clientOn = false;
bool clientIsConnected = false;

////**** Http ****//////////
String AccountLogin = "";
String AccountPassword = "";
String UniqueId = "";
String webServerUrl = "http://192.168.0.102:8000/";
String deviceModelName = "TermoController-battery v1.0";
bool changeWebTargetTemp = false;


ESP8266WebServer server(80);
IPAddress apIP(192, 168, 0, 150);
IPAddress localIP(192, 168, 0, 150);
IPAddress routerIP(192,168,0,1);
////***ADDRESS****/////

#define A_DATETIME       0            // 6 byte
#define A_KWT            6            //   float
#define A_NEED_TEMP      10           //   float
#define A_TFT_ACTIVE     14           //int
#define A_WATTS          16           //Int
#define A_HYSTERESIS     18          //float 
#define A_STATISTIC      22          // 28 byte
#define A_SSID           50          // 32 byte
#define A_PASSWORD       82          // 32 byte
#define A_LOGIN          114
#define A_SITE_PASSWORD  146
#define A_UNIQUE_ID      178         // 36 byte

////***DEFAULT PARAMS****////

unsigned long powerTime = 0;   //seconds
float needTemp = 22.0f;
int tftActiveTime = 60;
int watts = 2000;
float hysteresis = 0.1;
float KWt = 0.0;
////****************/////
#define RELE_PIN   16

bool isWork = false;
bool needTempChanged = false;


int settingSelect = -1;
float statistic[7] = {1,2,1,3,1,3,2}; 

bool tftChangeMode = false;

byte currentDay;

const char* brandName = "SmaHou";
const char* deviceName = "Termo_R.00";

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  //setTime(0, 0, 0, 1, 1, 2018);
  //SaveDateTime();
  LoadDateTime();
  currentDay = day();
  dht.begin();
  tftSetup();
  btnSetup();
  //pinMode(RELE_PIN, OUTPUT);
  Serial.println("Init finished");
  //
  //SaveParams();
  //SaveStatistic(A_STATISTIC);
  LoadParams();
}
void loop()
{
  checkWiFi();
  if(ApIsCreated || clientIsConnected)
  {
    if(loopTimer + LOOP_DELAY > millis())
      return;
    loopTimer = millis();
  }
  HttpLoop();
  checkSettingBlink();
  checkBattery();
  dhtGetData();
  checkBtn();
  checkActive();
  checkKWt();
  checkTftUpdate();
  checkDay();
}


///***********************///
///ANOTHER
///***********************///

float getKWtH(int timeSec)
{
  return timeSec/60.0/60.0*watts/1000.0;
}
int getSecond(long milsec)
{
  return milsec/1000;
}
float getMax(float a[])
{
   float max = -100000;
   for(int i=0; i<sizeof(a); i++)
    if(a[i] > max)
      max = a[i];
   return max;
}
float getMin(float a[])
{
   float min = 100000;
   for(int i=0; i<sizeof(a); i++)
    if(a[i] < min)
      min = a[i];
   return min;
}


///************************///
///INITIALIZATION
///************************///
void resetStatistic()
{
  for(int i=0; i<7; i++)
    statistic[i] = 0;
  SaveStatistic(A_STATISTIC);
}
void LoadParams()
{
  KWt = LoadFloat(A_KWT);  
  needTemp = LoadFloat(A_NEED_TEMP); 
  tftActiveTime = LoadInt(A_TFT_ACTIVE);
  watts = LoadInt(A_WATTS);
  hysteresis = LoadFloat(A_HYSTERESIS);
  LoadStatistic(A_STATISTIC);
  clientSSID = LoadString(A_SSID);
  clientPassword = LoadString(A_PASSWORD);
  AccountLogin = LoadString(A_LOGIN);
  AccountPassword = LoadString(A_SITE_PASSWORD);
  UniqueId = LoadString36(A_UNIQUE_ID);
}
void resetParams()
{
  needTemp = 22.0f;
  tftActiveTime = 60;
  watts = 2000;
  hysteresis = 0.1;
  KWt = 0.0;
  SaveParams();
}
void SaveParams()
{
  SaveFloat(A_KWT, KWt);
  SaveFloat(A_NEED_TEMP, needTemp); 
  SaveInt(A_TFT_ACTIVE, tftActiveTime);
  SaveInt(A_WATTS, watts);
  SaveFloat(A_HYSTERESIS, hysteresis);
  SaveString(A_SSID, clientSSID);
  SaveString(A_PASSWORD, clientPassword);
  SaveString(A_LOGIN, AccountLogin);
  SaveString(A_SITE_PASSWORD, AccountPassword);
  SaveString36(A_UNIQUE_ID, UniqueId);
}
void btnSetup()
{
  pinMode(BTN_PIN, INPUT);
}
void tftSetup()
{
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, LOW);
  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST7735_BLACK);
}
