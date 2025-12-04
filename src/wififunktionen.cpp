#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>

#include "wififunktionen.h"
#include "defs.h"

String ssid[3];
String password[3];
String espHostname;

//SPIFF ********************************************************************
//SPIFF ********************************************************************
String readFile(fs::FS &fs, const char * path)
{
  Serial.printf("Reading file: %s >>", path);
  
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Writing file: %s >> %s \r\n", path, message);

  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}
//*************************************************************************************


//*************************************************************************************
// HTML web page SETUP Wifi Parameters 
const char setupWifi_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<title>SetupWifi</title>
<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'>

  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
  </style>	
</head>

<body>
<h1>WIFI Setup</h1>
<form action="/SetupWifi"  method="post">

<table align=center border=0>
<tr>
    <td align=left>(1) Wifi/PWD : <br><br><td><input type="text" name="ssid1" value="%ssid1%"><br><br> <td><input type="password" name="pwd1" value="%pwd1%"><br><br> 
<tr>
    <td align=left>(2) Wifi/PWD : <br><br><td><input type="text" name="ssid2" value="%ssid2%"><br><br> <td><input type="password" name="pwd2" value="%pwd2%"><br><br> 
<tr>
    <td align=left>Hostname : <br><br><td><input type="text" name="hostname" value="%Hostname%"><br><br> <td><br><br> 
</table>     
	
<br><input type="submit" value="Submit">

</form><br>
<br> <a href="\">Return to Home Page</a>
</body>
</html>

)rawliteral";

// Replaces placeholder with button section in your web page
String processorSetupWIFI(const String& var){
  //Serial.println(var);
  if(var == "ssid1"){
    return readFile(SPIFFS, "/wifissid1.txt");
  }
  else if(var == "pwd1"){
    return readFile(SPIFFS, "/wifipwd1.txt");
  }
  else if(var == "ssid2"){
    return readFile(SPIFFS, "/wifissid2.txt");
  }
  else if(var == "pwd2"){
    return readFile(SPIFFS, "/wifipwd2.txt");
  }
  else if(var == "ssid3"){
    return ssid[2];
  }
  else if(var == "pwd3"){
    return password[2];
  }
  else if(var == "Hostname"){
    return readFile(SPIFFS, "/espHostname.txt");
  }
  return String();
}

void handlegetSetupWIFI(AsyncWebServerRequest *request) 
  {
    Serial.println("##WEBPAGE handlegetSetupWIFI");

    /*
    //List all parameters (Compatibility)
    int args = request->args();
    for(int i=0;i<args;i++){
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
    */

    String inputMessage;
    const char* TXT_ssid1 = "ssid1"; const char* TXT_pwd1 = "pwd1"; 
    const char* TXT_ssid2 = "ssid2"; const char* TXT_pwd2 = "pwd2";
    const char* TXT_hostname = "hostname";

    // GET input1 value on <ESP_IP>/getSetupWifi?ssid1=&pwd1=&ssid2=&pwd2=&ssid3=&pwd3=
    inputMessage = request->getParam(TXT_ssid1, true)->value();
    if ( !inputMessage.isEmpty()  )
    { writeFile(SPIFFS, "/wifissid1.txt", inputMessage.c_str());  }

    inputMessage = request->getParam(TXT_pwd1, true)->value();
    if ( !inputMessage.isEmpty()  )
    { writeFile(SPIFFS, "/wifipwd1.txt", inputMessage.c_str());  }
    
    inputMessage = request->getParam(TXT_ssid2, true)->value();
    if ( !inputMessage.isEmpty()  )
    { writeFile(SPIFFS, "/wifissid2.txt", inputMessage.c_str());  }
    
    inputMessage = request->getParam(TXT_pwd2, true)->value();
    if ( !inputMessage.isEmpty()  )
    { writeFile(SPIFFS, "/wifipwd2.txt", inputMessage.c_str());  }

    inputMessage = request->getParam(TXT_hostname, true)->value();
    if ( !inputMessage.isEmpty()  )
    { writeFile(SPIFFS, "/espHostname.txt", inputMessage.c_str());  }

    yield(); 
    request->send(200, "text/html", "<html><meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'><body><h4>WIFI SETUP SAVED! <br><br> <h3><a href='/'>Return to Home Page</a></body></html>");
    
  }

void handleSetupWIFI(AsyncWebServerRequest *request) 
  {
    Serial.println("##WEBPAGE handleSetupWIFI");
    request->send_P(200, "text/html", setupWifi_html, processorSetupWIFI);
  }


//connectToAP Start an AP
ushort WifiFunktionen::connectToAP(void)
{
    Serial.println(""); Serial.println("AP Start ...");
    WiFi.mode(WIFI_AP);
    
    delay(100);
    IPAddress Ip(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPsetHostname("SmartLamp");
    WiFi.softAPConfig(Ip, Ip, NMask);
        
    String APssid="SmartLamp_";         
    APssid=APssid+WiFi.macAddress();
 
    delay(200);  
    if (!WiFi.softAP(APssid.c_str()))     
    {
        Serial.println("Soft AP creation failed.");
    }
    else
    {
      delay(50);
      Serial.println("");
      Serial.println("LuftCheck-AccesPoint in Betrieb! ");
      Serial.print("SSID: "); Serial.println(WiFi.softAPSSID());
      Serial.print("IP: "); Serial.println(WiFi.softAPIP());
      Serial.print("MAC: "); Serial.println(WiFi.macAddress());
      Serial.print("connections: "); Serial.println(WiFi.softAPgetStationNum());
      Serial.println("");
    }

    return WiFi.getMode();
      //if (WiFi.getMode() == WIFI_MODE_AP)

}


//connectToWifi Start ans check connection to Wifi
ushort WifiFunktionen::connectToWifi(void)
{
  uint8_t maxWifiTry=10;
  ssid[2]=mySSID;
  password[2]=myPWD;
  bool AP_Betrieb=false;
  Serial.print(">>WifiFunktionen::connectToWifi  - WiFi.status: "); Serial.println(WiFi.status());

  if ( WiFi.status() != WL_CONNECTED )   //Bereits verbunden?
  {
    Serial.println(">>WifiFunktionen::connectToWifi  - WiFi nicht WL_CONNECTED");
    //SSID unf PWD füllen
    ssid[0]    =  readFile(SPIFFS, "/wifissid1.txt");
    password[0] = readFile(SPIFFS, "/wifipwd1.txt");
    ssid[1]    =  readFile(SPIFFS, "/wifissid2.txt");
    password[1] = readFile(SPIFFS, "/wifipwd2.txt");
    espHostname = readFile(SPIFFS, "/espHostname.txt");

    //Leerzeichen entfernen
    ssid[0].trim(); password[0].trim(); ssid[1].trim(); password[1].trim(); espHostname.trim();

    if (espHostname=="") { espHostname=myHOSTNAME; }
    Serial.print("ESP-Hostname: "); Serial.println(espHostname);
  
    // WiFi.scanNetworks will return the number of networks found
    Serial.print("WiFi.scanNetworks ...");
    int n = WiFi.scanNetworks();
    Serial.println(n);

    if (n>0)
    {
      for (int i = 0; i < n; ++i) {
          // Print SSID and RSSI for each network found
          Serial.print(i );
          Serial.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")  ");
          delay(2);
        }

      //höchste SSID der bekannten SSIDs ermitteln
      int max=-990;
      int max_i=250;

      for(size_t i = 0; i < n; ++i)
      {
        if( (WiFi.RSSI(i) > max)  && ( ( strcmp( WiFi.SSID(i).c_str() , ssid[0].c_str()) == 0 ) || ( strcmp( WiFi.SSID(i).c_str() , ssid[1].c_str()) == 0 ) ) )
        {
          max = WiFi.RSSI(i); //Übernimmt das Maximum
          max_i=i;
          Serial.printf("\n CHECK RSSI (nr: %i): ", i);
          Serial.print(WiFi.SSID(i)); Serial.print(" ("); Serial.print(WiFi.RSSI(i)); Serial.println(")");
        }
      }
      Serial.printf("\n --> max bekanntes LAN RSSI: %i ", max_i);
      Serial.print(WiFi.SSID(max_i)); Serial.print(" ("); Serial.print(WiFi.RSSI(max_i)); Serial.println(")");

      //Mit besten bekannten Lan verbinden
      int8_t connect_i=0;
      if      ( strcmp( WiFi.SSID(max_i).c_str() , ssid[0].c_str()) == 0 )    { connect_i=0; }
      else if ( strcmp( WiFi.SSID(max_i).c_str() , ssid[1].c_str()) == 0 )    { connect_i=1; }
      else                                                                    { connect_i=2; }

      //********************************************************
      //VERSION für nicht Sichtbare WLAN
      if (WiFi.status() != WL_CONNECTED)
      {
        if (max_i==250)  { connect_i=0; Serial.println("Passendes Wifi nicht gefunden, Versuche WIFI 0!"); }

        Serial.printf("WIFI connection gewählt: %i \n", connect_i);

        yield();
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(espHostname.c_str());
        WiFi.disconnect();
        delay(150);
        Serial.printf("WIFI connecting to %s : ", ssid[connect_i].c_str() );
        WiFi.begin(ssid[connect_i].c_str(), password[connect_i].c_str());
      
        uint8_t WifiTry=0;
        while ((WiFi.status() != WL_CONNECTED) && (WifiTry<maxWifiTry)) 
        {
          delay(1500); WifiTry++; Serial.printf("%u ", WifiTry);
        }
      }

      if (WiFi.status() != WL_CONNECTED)
      {
        if (max_i==250)  { connect_i=1; Serial.println("Passendes Wifi nicht gefunden, Versuche WIFI 1!"); }

        Serial.printf("WIFI connection gewählt: %i \n", connect_i);

        yield();
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(espHostname.c_str());
        WiFi.disconnect();
        delay(150);
        Serial.printf("WIFI connecting to %s : ", ssid[connect_i].c_str() );
        WiFi.begin(ssid[connect_i].c_str(), password[connect_i].c_str());
      
        uint8_t WifiTry=0;
        while ((WiFi.status() != WL_CONNECTED) && (WifiTry<maxWifiTry)) 
        {
          delay(1500); WifiTry++; Serial.printf("%u ", WifiTry);
        }
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print(">> WIFI connected: "); Serial.print(WiFi.SSID()); Serial.print(" "); Serial.print(WiFi.localIP());
        Serial.print("  RRSI: "); Serial.println(WiFi.RSSI());
      }
      else
      {
        Serial.println("WIFI NOT connected!!    **** ERROR **** ");  WiFi.disconnect();
      }

    }
    else
    {
      Serial.print("WIFI already connected: "); Serial.print(WiFi.SSID()); Serial.print(" "); Serial.println(WiFi.localIP());
    }
  }  //Kein WifiGefunden
  else
  { Serial.println("WiFI WL_CONNECTED");  }

  return WiFi.status();
}