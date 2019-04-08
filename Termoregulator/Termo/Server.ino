// Create an instance of the server
// specify the port to listen on as an argument
#define HANDLE_DELAY 50

unsigned long int serverTimer = 0;

void CreateServer(){
   handleSetup();
   server.begin();
}
void StopServer(){
  server.stop();
}
void handleSetup()
{
  server.on("/", handleRoot);
  server.on("/SettingsData/", handleSendSettingsData);
  server.on("/SetSettingsData", handleSetSettingsData);
  server.on("/resetKWatts/", hundleResetKWatts);
  server.on("/resetParams/", hundleResetParams);
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}

/////************////////
//   HUNDLES
/////*************////////

void handleRoot()
{
  server.send(200, "text/html", String(brandName) + " products: " + String(deviceName));
}
void handleSendSettingsData()
{
  String message = "";
  if (ApIsCreated)
    message += "localIP="+IpAddress2String(WiFi.softAPIP()) + ";";
  else
    message += "localIP="+IpAddress2String(WiFi.localIP()) + ";";
  message += "tftActiveTime="+String(tftActiveTime) + ";";
  message += "hysteresis="+String(hysteresis, 1) + ";";
  message += "watts="+String(watts) + ";";
  message += "WiFiSSID="+clientSSID + ";";
  message += "WiFiPassword="+clientPassword + ";";
  message += "AccountLogin="+AccountLogin + ";";
  message += "AccountPassword="+AccountPassword + ";"; 

  server.send(200, "text/html", message);
}
void handleSetSettingsData()
{
  /*String message = "Number of args received:";
  message += server.args();            //Get number of parameters
  message += "\n";                            //Add a new line
  
  for (int i = 0; i < server.args(); i++) {
  
    message += "Arg nº" + (String)i + " –> ";   //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter

  }
  Serial.println(message);*/
  for (int i=0; i<server.args(); i++){
    Serial.print(server.argName(i));
    Serial.print(" = " + server.arg(i));
    Serial.println();
    String key = server.argName(i);
     if (key == "tftActiveTime")
      tftActiveTime = server.arg(i).toInt();
     else if (key == "hysteresis")
      hysteresis = server.arg(i).toFloat();
     else if (key == "watts")
      watts = server.arg(i).toInt();
     else if (key == "WiFiSSID")
      clientSSID = server.arg(i);
     else if (key == "WiFiPassword")
      clientPassword = server.arg(i);
     else if (key == "AccountLogin")
      AccountLogin = server.arg(i);
     else if (key == "AccountPassword")
      AccountPassword = server.arg(i);
     else if (key == "date")
      setDate(server.arg(i));
     else if (key == "time")
      SetTime(server.arg(i));

  }
  SaveParams();
  SaveDateTime();
  tftSettingsChanged();
  server.send(200, "text/html", "Data changed");       //Response to the HTTP request
}
void hundleResetKWatts()
{
  KWt = 0;
  SaveFloat(A_KWT, KWt);
  tftUpdateData();
  server.send(200, "text/html", "OK");
}
void hundleResetParams()
{
  resetParams();
  tftSettingsChanged();
  server.send(200, "text/html", "OK");
}
//////////////////////////
//////////////////////////
//////////////////////////
void checkRequest() {
  if(!ApIsCreated && !clientIsConnected)
    return;
  if (serverTimer + HANDLE_DELAY < millis()){
    serverTimer = millis();
    server.handleClient();
  }
  
}