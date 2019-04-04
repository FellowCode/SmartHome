#define CONNECT_CHECK_DELAY   500
unsigned long connectTimer = 0;
int connectionCounter = 0;

char clSSID[32];
char clPassword[32];



void CreateAP()
{
  WiFiOn = true;
  //WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  //WiFi.config(apIP,routerIP,IPAddress(255,255,255,0),routerIP);
  ApIsCreated = WiFi.softAP(base_ssid, base_password);
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
  WiFiOn = true;
  clientOn = true;
  clientIsConnected = false;
  //WiFi.mode(WIFI_STA);
  /*serverIsCreated = WiFi.softAP(base_ssid, base_password);
   delay(100);*/
  clientSSID.toCharArray(clSSID,32);
  clientPassword.toCharArray(clPassword,32);
  Serial.println("SSID: "+ clientSSID);
  Serial.println("Password: " + clientPassword);
  WiFi.begin(clSSID, clPassword);
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
}
void WiFiClientLoop()
{
  if (!clientOn)
    return;
  if (WiFi.status() != WL_CONNECTED && connectTimer + CONNECT_CHECK_DELAY < millis()){
    connectTimer = millis();
    Serial.print("*");
    return;
  }
  if (WiFi.status() != WL_CONNECTED)
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
