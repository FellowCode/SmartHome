#define TCP_RECONN_DELAY  30 //sec

#define TCP_SEND_DATA_DELAY   30//sec

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
    tcpTimer = millis();
    SendDataToServer();
  }
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
    while(fields.length() > 0){
      String field = SplitLeft(fields, ';');
      
      String key = SplitLeft(field, '='), value = SplitRight(field, '=');
      Serial.println(key + " = " + value);
      if (key == "target_temp"){
        needTemp = value.toFloat();
        needTempChanged = true;
      }
      else if (key == "string_id")
        UniqueId = value;
      fields = SplitRight(fields, ';');
   }
   SaveParams();
 }
 else if (command == "AuthError"){
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
  
  data.AddField("AccountLogin", AccountLogin);
  data.AddField("AccountPassword", AccountPassword);

  data.AddField("model_name", deviceModelName);
  data.AddField("temp", String(temp));
  data.AddField("target_temp", String(needTemp));
  data.AddField("humidity", String(humidity));
  data.AddField("KWatts", String(KWt));

  if (changeWebTargetTemp){
    data.AddField("ChangeTargetTemp", "True");
    changeWebTargetTemp = false;
  } else
    data.AddField("ChangeTargetTemp", "False");
    
  TcpSend(command + ":" + data.get());
}
