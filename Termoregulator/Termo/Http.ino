#define SEND_REQUEST_DELAY  30 //sec

unsigned long int httpTimer = 0;

HTTPClient http;

String split[2];

class PostData{
  String data;
  
  public: PostData(){
    data = "";
  }
  public: void AddField(String key, String value){
      if (data.length() > 0)
        data += "&";
      data += key + "=" + value;
    }
  public: String get(){
      return data;
    }
};

void HttpLoop(){
  if (clientIsConnected && httpTimer + SEND_REQUEST_DELAY*1000 < millis()){
    httpTimer = millis();
    SendWebRequest();
  }
}
void Split(String value, char c){
  split[0] = value.substring(0, value.indexOf(c));
  if (value.indexOf(c) > 0)
    split[1] = value.substring(value.indexOf(c)+1);
  else
    split[1] = "";
}
void SendWebRequest(){
  Serial.println("Send web request");
  PostData post = PostData();

  post.AddField("AccountLogin", AccountLogin);
  post.AddField("AccountPassword", AccountPassword);

  post.AddField("model_name", deviceModelName);
  post.AddField("temp", String(temp));
  post.AddField("target_temp", String(needTemp));
  post.AddField("humidity", String(humidity));
  post.AddField("KWatts", String(KWt));

  if (changeWebTargetTemp){
    post.AddField("ChangeTargetTemp", "True");
    changeWebTargetTemp = false;
  } else
    post.AddField("ChangeTargetTemp", "False");

  if (UniqueId == ""){
    post.AddField("init", "True");
  }
  else{
    post.AddField("string_id", UniqueId);
  }
  http.begin(webServerUrl + "device/setdata/");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(post.get());   //Send the request
  if (httpCode == 200){
    String payload = http.getString();    //Get the response payload

    Split(payload, ':');
    if (split[0] == "Auth Error"){
      Error = true;
      tftUpdateData();
    }
    else if (split[0] == "Change configuration"){
      while(split[1].length() > 0){
        Split(split[1], ';');
        //Временное хранение оставшихся полей
        String tmp = split[1];
        //Разделение на ключ значение
        Split(split[0], '=');
        String key = split[0], value = split[1];
        Serial.println(key + " = " + value);
        if (key == "target_temp"){
          needTemp = value.toFloat();
          needTempChanged = true;
        }
        else if (key == "string_id")
          UniqueId = value;
        split[1] = tmp;
     }
     Error = false;
     tftUpdateData();
     SaveParams();
    }
  }
  else{
    Error = true;
    tftUpdateData();
  }
}
