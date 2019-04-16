void resetParams()
{
  needTemp = 22.0f;
  tftActiveTime = 60;
  watts = 2000;
  hysteresis = 0.1;
  KWt = 0.0;
  clientSSID = "";
  clientPassword = "";
  api_key = "";
  secret_key = "";
  WiFiOn = false;
  enable = ENABLED;
  SaveParams();
  ESP.restart();
}
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
  UniqueId = LoadString36(A_UNIQUE_ID);
  api_key = LoadString(A_API_KEY);
  secret_key = LoadString(A_SECRET_KEY);
  WiFiOn = LoadByte(A_WIFI_STATUS) == 1;
  enable = LoadByte(A_ENABLE);
  serverAddress = LoadIpAddress(A_SERVER_IP);
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
  SaveString36(A_UNIQUE_ID, UniqueId);
  SaveString(A_API_KEY, api_key);
  SaveString(A_SECRET_KEY, secret_key);
  SaveByte(A_WIFI_STATUS, byte(WiFiOn && clientOn));
  SaveByte(A_ENABLE, enable);
  SaveIpAddress(A_SERVER_IP, serverAddress);
}
