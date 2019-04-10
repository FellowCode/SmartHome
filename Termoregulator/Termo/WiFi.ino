#define CONNECT_CHECK_DELAY   30 // Ожидание подключения

unsigned long connectTimer = 0;
unsigned long reconnectTimer = 0;
int connectionCounter = 0;

char clSSID[32];
char clPassword[32];

const char* ssid = "Wi-Fi Yota";
const char* password = "01544424";

ESP8266WiFiMulti WiFiMulti;

void CreateAP()
{
  WiFiOn = true;
  //WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  //WiFi.config(apIP,routerIP,IPAddress(255,255,255,0),routerIP);
  ApIsCreated = WiFi.softAP(ap_ssid, ap_password);
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
  CreateServer();
}
void StopAP()
{
  StopServer();
  WiFi.mode(WIFI_OFF);
  WiFiOn = false;
  ApIsCreated = false;
  Serial.println("server stoped");
}
void WiFiConnect()
{
   /*Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");*/
  WiFiOn = true;
  clientOn = true;
  clientIsConnected = false;
  WiFi.mode(WIFI_STA);
  /*serverIsCreated = WiFi.softAP(base_ssid, base_password);
   delay(100);*/
  //WiFi.disconnect(true);
  //WiFi.setAutoConnect(false);
  //WiFi.setPhyMode(WIFI_PHY_MODE_11G);
  
  //Serial.println(WiFi.getPhyMode());
  Serial.println("SSID: "+ String(clientSSID));
  Serial.println("Password: " + String(clientPassword));
  WiFiMulti = ESP8266WiFiMulti();
  WiFiMulti.addAP(clientSSID.c_str(), clientPassword.c_str());
  //WiFi.begin(ssid, password);
  //WiFi.printDiag(Serial);
  connectTimer = millis();
  SaveParams();
  
}
void WiFiDisconnect()
{
  connectionCounter = 0;
  Serial.println();
  server.stop();
  WiFi.mode(WIFI_OFF);
  WiFiOn = false;
  clientOn = false;
  clientIsConnected = false;
  tftUpdateData();
}
void WiFiClientLoop()
{
  if (!clientOn)
    return;
  if (WiFiMulti.run() != WL_CONNECTED && connectTimer + CONNECT_CHECK_DELAY*1000 < millis()){
    ESP.restart();
  }
  if (WiFiMulti.run() != WL_CONNECTED)
    return;
  if (!clientIsConnected){
    localIP = WiFi.localIP();
    Serial.println("\nconnected");
    Serial.println(localIP);
    clientIsConnected = true;
    CreateServer();
    tftUpdateData();
    
    return;
  }
}
