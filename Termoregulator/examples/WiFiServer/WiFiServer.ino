#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define CONNECT_REPEAT_DELAY   300;
unsigned long connectTimer;

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);
IPAddress apIP(192, 168, 0, 50);

const char* base_ssid = "Termo_R00v01";
const char* base_password = "12345678";

String brandName = "SmaHou";
String deviceName = "Termo_R.00";

bool WiFiOn = false;
bool serverIsCreated = false;

void CreateServer()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  bool result = WiFi.softAP(base_ssid, base_password);
  Serial.println(result);
  WiFiOn = true;
  handleSetup();
  serverIsCreated = result;
  server.begin();
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
}
void handleSetup()
{
  server.on("/", handleRoot);
  server.on("/SettingsData", handleSendSettingsData);
  server.on("/SetSettingsData", handleRoot);
}

/////************////////
//   HUNDLES
/////*************////////

void handleRoot()
{
  server.send(200, "text/plain", brandName + " products: " + deviceName);
}
void handleSendSettingsData()
{
  /*String message = "";
  message += getDate() + ";";
  message += getTime() + ";";
  message += String(tftActiveTime) + ";";
  message += String(hysteresis) + ";";
  message += String(watts) + ";"; 

  server.send(200, "text/plain", message);*/
}
void handleSettingsData()
{
  String message = "Number of args received:";
  message += server.args();            //Get number of parameters
  message += "\n";                            //Add a new line
  
  for (int i = 0; i < server.args(); i++) {
  
    message += "Arg nº" + (String)i + " –> ";   //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter

  } 
  server.send(200, "text/plain", "OK");       //Response to the HTTP request
}
//////////////////////////
//////////////////////////
//////////////////////////
void checkRequest() {
  if(!serverIsCreated)
    return;
  server.handleClient();
}
void StopServer()
{
  WiFi.mode(WIFI_OFF);
  WiFiOn = false;
  serverIsCreated = false;
}

void setup() {
  Serial.begin(9600);

  CreateServer();

}

void loop() {
  // put your main code here, to run repeatedly:
  checkRequest();
}
