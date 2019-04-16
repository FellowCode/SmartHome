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
#include <asyncHTTPrequest.h>
#include <Hash.h>

#define LOOP_DELAY  10
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
#define TFT_RST    5
#define TFT_LED    15

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

enum {MAIN, SETTINGS, STATISTIC};    //Sheets
enum {Date, Time, Display, Hysteresis, Watts, ResetWatts, WiFiSetup, ResetParams};
enum {WiFi_HOTSPOT, WiFi_CLIENT, SSID, Password, IP};

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

////**** Http/TCP ****//////////
String api_key = "";
String secret_key = "";
String UniqueId = "";
String webServerUrl = "http://192.168.0.102:8000/";

IPAddress serverAddress(62,109,29,169);

String deviceModelName = "TermoController-battery v1.0";
String deviceFirmware = "TermoC_v1";
bool changeWebTargetTemp = false;
bool Error = false;


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
#define A_API_KEY        114         // 32 byte
#define A_SECRET_KEY     146         // 32 byte
#define A_UNIQUE_ID      178         // 36 byte
#define A_WIFI_STATUS    214         // 1 byte
#define A_ENABLE         215         // 1 byte
#define A_SERVER_IP      216         // 4 byte

////***DEFAULT PARAMS****////

#define NEED_TEMP_CHANGE_REQ_DELAY    3 //sec

enum {DISABLED, ENABLED};

byte enable = ENABLED;
unsigned long powerTime = 0;   //seconds
unsigned long int needTempChangeTime = 0;
float needTemp = 22.0f;
int tftActiveTime = 60;
int watts = 2000;
float hysteresis = 0.1;
float KWt = 0.0;
////****************/////


bool isWork = false;
bool needTempChanged = false;


int settingSelect = -1;
float statistic[7] = {1,2,1,3,1,3,2}; 

bool tftChangeMode = false;

byte currentDay;

const char* brandName = "Guraton";
const char* deviceName = "Termo_R.00";
#include "user_interface.h"
void setup() {
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  /*if (resetInfo->reason != REASON_DEEP_SLEEP_AWAKE) {
    ESP.deepSleep(10, WAKE_RF_DEFAULT)
  }*/
  EEPROM.begin(512);
  Serial.begin(115200);
  RelaySetup();
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
  //serverAddress = IPAddress(62,109,29,169);
  if (WiFiOn){
    WiFiConnect();
  }
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
  //HttpLoop();
  TcpLoop();
  checkSettingBlink();
  checkBattery();
  dhtGetData();
  checkBtn();
  checkActive();
  checkKWt();
  checkTftUpdate();
  checkDay();
}

///************************///
///INITIALIZATION
///************************///


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
