#define TCP_RECONN_DELAY  30 //sec

#define TCP_SEND_DATA_DELAY   60//sec

unsigned long int tcpTimer = 0;
bool sendData = false;
String dataToSend = "";

String buffer = "";

bool connToServer = false;

WiFiClient client;

String SplitLeft(String value, char c){
  int i = value.indexOf(c);
  if (i>0)
    return  value.substring(0, i);
  else
    return value;
}
String SplitRight(String value, char c){
  int i = value.indexOf(c);
  if (i>0)
    return  value.substring(i+1);
  else
    return "";
}

void TcpLoop(){
  
  if (!clientIsConnected)
    return;
  if(!client.connected()){
    if (connToServer){
      connToServer = false;
      Serial.println("disconnect");
      tcpTimer = millis();
      Error = true;
      tftUpdateData();
    }

    //Переподключение
    if (tcpTimer + TCP_RECONN_DELAY*1000 < millis()){
      if(!client.connect(serverAddress, 10002)){
        tcpTimer = millis();
        Serial.println("Connect failed");
        Error = true;
        tftUpdateData();
      }
    }
    return;
  }
  if (!connToServer){
     Serial.println("Connected to server");
     connToServer = true;
     SendDataToServer();
     tcpTimer = millis();
     Error = false;
     tftUpdateData();
  }
  //Скидываем данные на сервер в определенном интервале
  if (tcpTimer + TCP_SEND_DATA_DELAY*1000 < millis()){
    SendDataToServer();
  }
  // Если температура была изменена отправляем на сервер
  if (changeWebTargetTemp && needTempChangeTime + NEED_TEMP_CHANGE_REQ_DELAY * 1000 < millis())
    SendDataToServer();
  //Отправляем сообщение, если есть
  if (sendData){
    client.println(dataToSend);
    sendData = false;
  }
  //Принимаем сообщение
  if (client.available()){
    char c = client.read();
    if (c!='\n')
      buffer += c;
    else
      TcpGet(buffer);
  }
}

void TcpSend(String data){
  sendData = true;
  dataToSend = data;
}

void TcpGet(String data){
  buffer = "";
  Serial.print("get: ");
  Serial.println(data);

  String command = SplitLeft(data, ':');

  if (command == "ChangeConfig"){
    String fields = SplitRight(data, ':');
    String checkStr = data.substring(0, data.lastIndexOf('&')) + ";" + secret_key;
    Serial.println("check: " + checkStr);

    float needTempTmp = -1;
    String UniqueIdTmp = "-1";
    byte enableTmp = 255;
    
    while(fields.length() > 0){
      String field = SplitLeft(fields, '&');
      
      String key = SplitLeft(field, '='), value = SplitRight(field, '=');
      Serial.println(key + " = " + value);
      if (key == "target_temp"){
        needTempTmp = value.toFloat();
      }
      else if (key == "string_id")
        UniqueIdTmp = value;
      else if (key == "enable"){
        if (value == "True")
          enableTmp = ENABLED;
        else
          enableTmp = DISABLED;
      }
      else if (key == "hash"){
        if (sha1(checkStr) == value){
          if (needTempTmp != -1){
            needTemp = needTempTmp;
            needTempChanged = true;
          }
          if (UniqueIdTmp != "-1")
            UniqueId = UniqueIdTmp;
          if (enableTmp != 255)
            enable = enableTmp;
        }
        else
          Serial.println("Hash not equals");
      }
      fields = SplitRight(fields, '&');
   }
   SaveParams();
 }
 else if (command == "AuthError" || command == "HashCheckError"){
  Error = true;
  tftUpdateData();
 }
 else if (command == "SendCurrentData"){
  SendDataToServer();
 }
}
void SendDataToServer(){
  String command = "ChangeValues";
  
  JsonData data;
  if (UniqueId == ""){
    data.AddField("init", "True");
  }
  else {
    data.AddField("string_id", UniqueId);
  }
  
  data.AddField("api_key", api_key);

  data.AddField("model_name", deviceModelName);
  data.AddField("temp", String(temp));
  data.AddField("target_temp", String(needTemp));
  data.AddField("humidity", String(humidity));
  data.AddField("KWatts", String(KWt));
  if (enable == ENABLED)
    data.AddField("enable", "True");
  else
    data.AddField("enable", "False");

  data.AddField("ChangeEnable", "False");

  if (changeWebTargetTemp){
    data.AddField("ChangeTargetTemp", "True");
    changeWebTargetTemp = false;
  } else
    data.AddField("ChangeTargetTemp", "False");

  data.AddField("hash", sha1(command + ":" + data.get() + ";" + secret_key));

  TcpSend(command + ":" + data.get());

  tcpTimer = millis();
}
