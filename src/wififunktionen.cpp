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
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WIFI Setup</title>
  <style>
    body {
      font-family: system-ui, sans-serif;
      background: #f4f7fa;
      color: #222;
      text-align: center;
      margin: 0;
      padding: 1.5em;
    }

    h1 {
      font-size: 1.8em;
      margin-bottom: 0.8em;
      color: #b30000;
    }

    form {
      background: white;
      border-radius: 12px;
      padding: 1.5em;
      max-width: 500px;
      margin: auto;
      box-shadow: 0 4px 8px rgba(0,0,0,0.1);
    }

    label {
      display: block;
      text-align: left;
      margin-top: 1em;
      font-weight: 600;
    }

    input[type="text"],
    input[type="password"] {
      width: 100%%;
      margin-top: 0.3em;
      padding: 0.8em;
      font-size: 1em;
      border: 1px solid #ccc;
      border-radius: 8px;
      box-sizing: border-box;
      background: #fff;
      color: #222;
    }

    input[readonly] {
      background: #eee;
      color: #666;
    }

    input[type="submit"] {
      width: 100%%;
      margin-top: 1.8em;
      padding: 0.9em;
      font-size: 1.1em;
      border: none;
      border-radius: 8px;
      background-color: #b30000;
      color: white;
      cursor: pointer;
      transition: background 0.2s;
    }

    input[type="submit"]:hover {
      background-color: #900000;
    }

    a {
      display: inline-block;
      margin-top: 1.5em;
      color: #b30000;
      text-decoration: none;
      font-size: 1em;
    }

    a:hover {
      text-decoration: underline;
    }

    /* Für sehr kleine Bildschirme */
    @media (max-width: 400px) {
      h1 { font-size: 1.5em; }
      label { font-size: 0.95em; }
      input { font-size: 0.95em; }
    }
  </style>
</head>

<body>
  <h1>WIFI Setup</h1>
  <form action="/SetupWifi" method="post">

    <label>(1) Wifi/PWD:</label>
    <input type="text" name="ssid1" value="%ssid1%">
    <input type="password" name="pwd1" value="%pwd1%">

    <label>(2) Wifi/PWD:</label>
    <input type="text" name="ssid2" value="%ssid2%">
    <input type="password" name="pwd2" value="%pwd2%">

    <label>Wifi/PWD:</label>
    <input type="text" name="s" value="%ssid3%" readonly>
    <input type="text" name="p" value="%pwd3%" readonly>

    <label>Hostname:</label>
    <input type="text" name="hostname" value="%Hostname%">

    <input type="submit" value="Speichern">
  </form>

  <a href="\">Zurück zur Startseite</a>
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
    String Hostname = readFile(SPIFFS, "/espHostname.txt");
    if (Hostname=="") { Hostname=myHOSTNAME; }
    return Hostname;
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
    request->send(200, "text/html", "<html><meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'> <body><br><h4>Einstellungen gespeichert! <br> <h3><a href='/'>Startseite</a> <br> <h3><a href='/SetupWifi'>SetupWifi</a> </body></html>");
      
    String  HtmlPage="<html><meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'> \n";
            HtmlPage+="<style>html {font-family: Arial; display: inline-block; text-align: center;}</style> \n";
            HtmlPage+="<body><br><h4>Einstellungen gespeichert! <br> <h3><a href='/'>Startseite</a> <br> <h3><a href='/setup'>Setup</a> </body></html>";
    request->send(200, "text/html", HtmlPage);

  }

void handleSetupWIFI(AsyncWebServerRequest *request) 
  {
    Serial.println("##WEBPAGE handleSetupWIFI");
    request->send(200, "text/html", setupWifi_html, processorSetupWIFI);
  }

  //WifiOFF
void WifiFunktionen::WifiOFF(void)
{
  WiFi.disconnect(true); 
  WiFi.softAPdisconnect(true);
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
}

//connectToWifi Start ans check connection to Wifi
ushort WifiFunktionen::connectToWifi(void)
{
  uint8_t maxWifiTry=10;
  
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

    ssid[2] = mySSID;
    password[2] = myPWD;

    //Leerzeichen entfernen
    ssid[0].trim(); password[0].trim(); ssid[1].trim(); password[1].trim(); espHostname.trim();

    if (espHostname=="") { espHostname=myHOSTNAME; }
    Serial.print("ESP-Hostname: "); Serial.println(espHostname);
  
    /*
    // WiFi.scanNetworks will return the number of networks found
    Serial.print("WiFi.scanNetworks ...");
    int n = WiFi.scanNetworks();
    Serial.println(n);

    if (n > 0)
    {
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        Serial.print(i);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.println(")  ");
      }
    }
    */

    int8_t connect_i = 0;
    uint8_t WifiTry = 0;

    for (connect_i = 0; connect_i < 3; ++connect_i)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        WifiTry = 0;
        Serial.printf("\n \nWIFI connection %i \n", connect_i);

        yield();
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(espHostname.c_str());
        WiFi.disconnect();
        delay(450);
        
        Serial.printf("WIFI connecting to %s : ", ssid[connect_i].c_str());
        //Serial.printf("  PWD %s : ", password[connect_i].c_str());
        
        WiFi.begin(ssid[connect_i].c_str(), password[connect_i].c_str());

        while ((WiFi.status() != WL_CONNECTED) && (WifiTry < maxWifiTry))
        {
          delay(1500);
          WifiTry++;
          Serial.printf("%u ", WifiTry);
        }
      }
    }


    // Endkontrolle
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("\n>> WIFI connected: ");
      Serial.print(WiFi.SSID());
      Serial.print("  IP: ");
      Serial.print(WiFi.localIP());
      Serial.print("  RRSI: ");
      Serial.println(WiFi.RSSI());
      Serial.print(">> MAC: ");
      Serial.println(WiFi.macAddress());
      Serial.println(" ");

      /*Serial.print(macadr[0], HEX);
      Serial.print(":");
      Serial.print(macadr[1], HEX);
      Serial.print(":");
      Serial.print(macadr[2], HEX);
      Serial.print(":");
      Serial.print(macadr[3], HEX);
      Serial.print(":");
      Serial.print(macadr[4], HEX);
      Serial.print(":");
      Serial.println(macadr[5], HEX);                    */

      Serial.println("WIFI connected!!");
    }
    else
    {
      Serial.println("WIFI NOT connected!!    **** ERROR **** ");
      WiFi.disconnect();


      //AP ERSTELLEN
        yield();
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        delay(450);

        // MAC-Adresse des AP-Interfaces holen
        String mac = WiFi.softAPmacAddress();
        mac.replace(":", ""); // Doppelpunkte entfernen

        // SSID mit MAC-Adresse zusammensetzen
        String ssid = myHOSTNAME + mac;
        ssid.replace(" ", "_");

        // Access Point mit fixer IP starten
        // Statische IP-Konfiguration
        IPAddress local_IP(192, 168, 0, 1);
        IPAddress gateway(192, 168, 0, 1);
        IPAddress subnet(255, 255, 255, 0);
        if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
          Serial.println("Fehler: Konnte AP-IP nicht konfigurieren!");
        }

        // Access Point OHNE Passwort starten (offenes WLAN)
        if (!WiFi.softAP(ssid.c_str())) {
          Serial.println("Fehler: Konnte offenen Access Point nicht starten!");
        }

        Serial.println("Access Point gestartet!");
        Serial.print("SSID: ");
        Serial.println(ssid);
        Serial.print("IP-Adresse: ");
        Serial.println(WiFi.softAPIP());
        Serial.print("MAC-Adresse: ");
        Serial.println(WiFi.softAPmacAddress());
        Serial.print("Wifi STATUS: ");
        Serial.println(WiFi.status());


    }

  } // Kein WifiGefunden
  else
  {
    Serial.print("WIFI connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.localIP());
  }

  return WiFi.status();
}