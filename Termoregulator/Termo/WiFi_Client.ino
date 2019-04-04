#define CONNECT_CHECK_DELAY   200
unsigned long connectTimer = 0;

char clSSID[32];
char clPassword[32];

bool clientOn = false;

void WiFiConnect()
{
  WiFiOn = true;
  clientOn = true;
  clientIsConnected = false;
  tftUpdateData();
  //WiFi.mode(WIFI_STA);
  WiFi.config(apIP,routerIP,IPAddress(255,255,255,0),routerIP);
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
  if (WiFi.status() != WL_CONNECTED && connectTimer + CONNECT_CHECK_DELAY < millis()){
    connectTimer = millis();
    Serial.print("*");
    return;
  }
  if (WiFi.status() != WL_CONNECTED)
    return;
  if (!clientIsConnected){
    Serial.println("\nconnected");
    Serial.println(WiFi.localIP());
    clientIsConnected = true;
    tftUpdateData();
    return;
  }
}
