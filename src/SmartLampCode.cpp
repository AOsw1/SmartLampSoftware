//https://github.com/knolleary/pubsubclient/tree/master

#include <Arduino.h>
using namespace std;

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
 
#include "WIFIfunktionen.h"
#include "defs.h"
#include "values.h"

WifiFunktionen myWifi;
AsyncWebServer server(80);

#include "Webpages.h"

//MQTT Schnittstelle
#include <PubSubClient.h>
//const char* mqtt_server = mqttLink;  // Add your MQTT Broker IP address
WiFiClient mqttEspClient;
PubSubClient mqttclient(mqttEspClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String mqttLink;
String mqttUser;
String mqttKW;
String mqttTopic;
int  mqttIntervall;
bool mqtt_reconnect();
void mqtt_callback(char* topic, byte* message, unsigned int length);
void publishMQTT(void);
bool DoMQTT=false;

//Preferenzes zum Werte abspeichern
#include <Preferences.h>
Preferences preferences;

// Global Variables
unsigned long previousMillis = 0;
unsigned long interval1min = 60000*1;       //1 min
unsigned long interval5min = 60000*5;       //5 min
unsigned long interval10min = 60000*10;     //10 min
unsigned long interval30Min = 60000*30;     //30 min
unsigned long interval2Std = 60000*60*2;    //2Std
unsigned long interval4Std = 60000*60*4;    //4Std
unsigned long currentMillis = 0;
unsigned long previousMillisPublMQTT = 0;

unsigned int level_R = 0;
unsigned int level_G = 0;
unsigned int level_B = 0;
unsigned int level_Rsave = 0;
unsigned int level_Gsave = 0;
unsigned int level_Bsave = 0;

int i = 0; // Index für Light-Level
boolean ledEIN = 0;    // Flag für LED ist ON/OFF
String hexstring = "";
String ein = "EIN";
String aus = "AUS";
String save = "SAVE";
String recall = "RECALL";
String dunkler = "DUNKLER";
String heller = "HELLER";
String rgb = "rgb";

byte macadr[6];             // MAC address from WemosD1

String rgb_werte ="";
String WEBcommand = "";
String espBeschreibung;

//Funktionsprototypen
void SetLight(int myR, int myG, int myB);
void BlinkLight(int Anz, int myR, int myG, int myB);
void SetLEDein(int SetTo);
void SetLEDein(int SetTo, bool msg);
void handleRoot(AsyncWebServerRequest *request);
void handleledStatus(AsyncWebServerRequest *request);
void handlenotFound(AsyncWebServerRequest *request);
void handleSetup(AsyncWebServerRequest *request);
String processor(const String& var);
void LedStartUp(void);

enum ClickArt {keinClick, kurzClick, langClick, doppelClick};
ClickArt Click=keinClick;

enum RotArt {rRot=1, gRot=2, bRot=3, allColRot=4};
RotArt& operator++(RotArt& d)
{
  return d = (d == RotArt::allColRot) ? RotArt::rRot : static_cast<RotArt>(static_cast<int>(d)+1);
}
RotArt RotEncoder =allColRot;

  
void setup() {
      
  Serial.begin(115200);
  delay(100);
  Serial.println("\n \n PROGRAMMSTART ************************************* \n");
  Serial.print("SWver "); Serial.println(SWver);

  //=====================================================================
  pinMode(LED_PIN_NANO_B, OUTPUT);
  pinMode(LED_PIN_NANO_G, OUTPUT);
  pinMode(LED_PIN_NANO_R, OUTPUT);

  pinMode(ROT_SW, INPUT);   //PullUp R auf der Platine
  pinMode(ROT_A, INPUT);
  pinMode(ROT_B, INPUT);

  // Setup PWM and attach the channel to the GPIO to be controlled
  ledcSetup(ledChannelR, freq, resolution);
  ledcSetup(ledChannelG, freq, resolution);
  ledcSetup(ledChannelB, freq, resolution);
  ledcAttachPin(LED_PIN_NANO_R, ledChannelR);
  ledcAttachPin(LED_PIN_NANO_G, ledChannelG);
  ledcAttachPin(LED_PIN_NANO_B, ledChannelB);
  
  LedStartUp();

  Serial.println(); Serial.println();Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("Smart Lamp"));
  
  // Interrupt nicht in Verwendung
  // Initialize interrupt service routine
  //attachInterrupt(digitalPinToInterrupt(ROT_SW), interruptRoutine, FALLING);
  
  //=====================================================================
  if(!SPIFFS.begin(true)){ Serial.println("Error mounting SPIFFS"); }    //return;}
  else                 { Serial.println("Mounting SPIFFS OK!");  }
  
  preferences.begin("calibration", true);
  mqttLink = preferences.getString("mqttLink", "192.168.0.166");     Serial.printf("mqttLink: %s \n", mqttLink.c_str() );
  mqttUser = preferences.getString("mqttUser", "Mqttuser");            Serial.printf("mqttUser: %s \n", mqttUser.c_str() );
  mqttKW = preferences.getString("mqttKW", "MqttKW");             Serial.printf("mqttKW: %s \n", mqttKW.c_str() );
  mqttTopic = preferences.getString("mqttTopic", "SmartLamp");       Serial.printf("mqttTopic: %s \n", mqttTopic.c_str() );
  mqttIntervall = preferences.getInt("mqttIntervall", 0);            Serial.printf("mqttIntervall: %i \n", mqttIntervall);
  preferences.end();
  Serial.println();

  SetLight(0,0,50);
  WiFi.persistent(false);
  WiFi.disconnect(true); 
  WiFi.mode(WIFI_OFF);
  delay(250);


  //AP-Modus erzwingen
  if (digitalRead(ROT_SW)==LOW )
  {
    Serial.println("MIT AP-Mode-Starten!");
    if(myWifi.connectToAP() == WIFI_MODE_AP)
    {
      BlinkLight(4,100,100,100);    //Wifi verbunden, WEISS BLINKEN
    }
  }
  else
  {
      if (myWifi.connectToWifi() == WL_CONNECTED)
      {
        BlinkLight(4,0,200,0);    //Wifi verbunden, GRÜN BLINKEN
      }
      else if(myWifi.connectToAP() == WIFI_MODE_AP)
      {
        BlinkLight(4,0,0,200);    //Wifi verbunden, BLAU BLINKEN
      }
      else
      {
        BlinkLight(4,200,0,0);    //Wifi verbunden, ROT BLINKEN
      }
  }

  // Route for SetupWifi
  server.on("/SetupWifi", HTTP_GET, handleSetupWIFI);
  server.on("/SetupWifi", HTTP_POST, handlegetSetupWIFI);
  
  // Route for SmartLamp
  server.on("/", HTTP_GET, handleRoot);
  server.on("/ledStatus", HTTP_GET, handleledStatus);
  server.onNotFound(handlenotFound);

  // Route for KALIBRATION
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request)     { request->send_P(200, "text/html", SETUP_html, processor); });
  server.on("/Setup", HTTP_GET, [](AsyncWebServerRequest *request)     { request->send_P(200, "text/html", SETUP_html, processor); });
  server.on("/setup", HTTP_POST, handleSetup);

  yield();
  server.begin();
  Serial.println("WEBSERVER gestartet");

  //MQTT starten
  if ( WiFi.status()  == WL_CONNECTED )
    {
      // MQTT
      Serial.print(CR "mqtt starteten   ");
      mqttclient.setServer(mqttLink.c_str(), 1883);
      mqttclient.setCallback(mqtt_callback); // Topics zum empfangen

      if (mqtt_reconnect()) // Mqtt verbinden
      {
        Serial.println("... mqtt gestartet");
        DoMQTT = true;
        publishMQTT();
      }
      else
      {
        Serial.println("... mqtt ERROR");
      }
  }
  else
  {
    Serial.println("... kein WIFI, kein mqtt");
  }

  Serial.println("SETUP complete...");
  level_R=20; level_G=20; level_B=20; 
  SetLight(level_R, level_G, level_B);
  ledEIN=1;
}

void SetLight(int myR, int myG, int myB)
{
    ledcWrite(ledChannelR, myR);
    ledcWrite(ledChannelG, myG);
    ledcWrite(ledChannelB, myB);
    level_Rsave=myR;
    level_Gsave=myG;
    level_Bsave=myB;
}

void BlinkLight(int Anz, int myR, int myG, int myB)
{
  int level_Rvorher=0;
  int level_Gvorher=0;
  int level_Bvorher=0;

  level_Rvorher = level_Rsave;
  level_Gvorher = level_Gsave;
  level_Bvorher = level_Bsave;
  
  for (int i=0; i<Anz; i++)
  {
    SetLight(myR, myG, myB);
    delay(150);
    SetLight(0, 0, 0);
    delay(150);
  }

  SetLight(level_Rvorher, level_Gvorher, level_Bvorher);
}

void SetLEDein(int SetTo)
{
  SetLEDein(SetTo, false);
}

void SetLEDein(int SetTo, bool msg)
{
  int i=0;

  if ((SetTo==setTOGGLE) && (ledEIN==1)) { SetTo=setAUS; }
  if ((SetTo==setTOGGLE) && (ledEIN==0)) { SetTo=setEIN; }

  switch ( SetTo ) 
  {
    case setEIN:
      Serial.println("setEIN");
      while (i<255) {    // Softstart Lampe ein
        SetLight(i, i, i);
        i++; i++;
        delay(3);
      }
      level_R =255; level_G =255; level_B =255;
      ledEIN = 1;
      break;

    case setAUS:
      Serial.println("setAUS");
      while ((level_Rsave > 5) || (level_Gsave > 5) || (level_Bsave > 5)) 
      {
        if (level_Rsave >= 5) {
          level_Rsave = level_Rsave - 2;
        }
        if (level_Gsave >= 5) {
          level_Gsave = level_Gsave - 2;
        }
        if (level_Bsave >= 5) {
          level_Bsave = level_Bsave - 2;
        }
        SetLight(level_Rsave, level_Gsave, level_Bsave);
        delay(3);
        //Serial.print("+");
      }

      Serial.print("AUS");

      // Licht aus
      level_R = 0; level_G = 0; level_B = 0;
      SetLight(level_R, level_G, level_B);
      ledEIN = 0;
      break;

    case setRotHELL:  
      if (ledEIN) 
      {
          if ((level_R < 239) && (RotEncoder==rRot))  {  level_R += 15;   }
          if ((level_G < 239) && (RotEncoder==gRot))  {  level_G += 15;   }
          if ((level_B < 239) && (RotEncoder==bRot))  {  level_B += 15;    }
      }
      else
      {
        if (RotEncoder==rRot)  {level_R =10;}
        if (RotEncoder==gRot)  {level_G =10;}
        if (RotEncoder==bRot)  {level_B =10;}
        ledEIN=1;
      }
      SetLight(level_R, level_G, level_B);  
      Serial.print("rotHELLER---R G B "); Serial.print(level_R);Serial.print(" ");Serial.print(level_G);Serial.print(" ");Serial.print(level_B);Serial.println(" ");   
      break;

    case setRotDUNKEL:
      if (ledEIN) {
        if ((level_R > 8)  && (RotEncoder==rRot)) {  level_R -= 5;   }
        if ((level_G > 8)  && (RotEncoder==gRot)) {  level_G -= 5;   }
        if ((level_B > 8)  && (RotEncoder==bRot)) {  level_B -= 5;    }
     
        SetLight(level_R, level_G, level_B);  
        Serial.print("rotDUNKLER---R G B "); Serial.print(level_R);Serial.print(" ");Serial.print(level_G);Serial.print(" ");Serial.print(level_B);Serial.println(" ");      
      }
      else { Serial.println("rotDUNKLER LED bereits aus"); }
      break;

    case setHELL:
      if (ledEIN) {
          if (level_R < 244) {  level_R += 10;   }
          if (level_G < 244) {  level_G += 10;   }
          if (level_B < 244) {  level_B += 10;    }
      }
      else
      {
        level_R =10; level_G =10; level_B =10;
        ledEIN=1;
      }
      SetLight(level_R, level_G, level_B);  
      Serial.print("HELLER---R G B "); Serial.print(level_R);Serial.print(" ");Serial.print(level_G);Serial.print(" ");Serial.print(level_B);Serial.println(" ");   
      break;

    case setDUNKEL:
      if (ledEIN) {
        if (level_R > 35) {  level_R -= 14;   }
        if (level_G > 35) {  level_G -= 14;   }
        if (level_B > 35) {  level_B -= 14;    }
        
        if ((level_R > 6)&&(level_R < 36)) {  level_R -= 5;   }
        if ((level_G > 6)&&(level_G < 36)) {  level_G -= 5;   }
        if ((level_B > 6)&&(level_B < 36)) {  level_B -= 5;    }

        SetLight(level_R, level_G, level_B);  
        Serial.print("DUNKLER---R G B "); Serial.print(level_R);Serial.print(" ");Serial.print(level_G);Serial.print(" ");Serial.print(level_B);Serial.println(" ");      
      }
      else { Serial.println("DUNKLER LED bereits aus"); }
      break; 

    case setSAVE:  // aktuelle Farben speichern
      if (ledEIN) 
      {
        BlinkLight(3,0,150,150);
        SetLight(level_R, level_G, level_B);

        String lightlevelstr = "ll";
        if      (level_R<10)  { lightlevelstr = lightlevelstr + "00" + level_R; }
        else if (level_R<100) { lightlevelstr = lightlevelstr + "0" + level_R; }
        else                  { lightlevelstr += level_R; }
        if      (level_G<10)  { lightlevelstr = lightlevelstr + "00" + level_G; }
        else if (level_G<100) { lightlevelstr = lightlevelstr + "0" + level_G; }
        else                  { lightlevelstr += level_G; }
        if      (level_B<10)  { lightlevelstr = lightlevelstr + "00" + level_B; }
        else if (level_B<100) { lightlevelstr = lightlevelstr + "0" + level_B; }
        else                  { lightlevelstr += level_B; }
        
        //Serial.printf("Inhalt von lightlevelstr: %s ",lightlevelstr.c_str());
        writeFile(SPIFFS, "/lightlevel.txt", lightlevelstr.c_str()  );  

        Serial.print("SAVE---R G B "); Serial.print(level_R);Serial.print(" ");Serial.print(level_G);Serial.print(" ");Serial.print(level_B);Serial.println(" ");      
      }
      break; 

    case setRECALL:  // aktuelle Farben lesen
      String lightlevelstr = readFile(SPIFFS, "/lightlevel.txt");
      Serial.printf("Inhalt von lightlevelstr: %s ",lightlevelstr.c_str());
 
      Serial.println("++++++++");
      Serial.println( lightlevelstr.substring(2,5).toInt() );     //ll010010010
      Serial.println( lightlevelstr.substring(5,8).toInt() );     //12345678901
      Serial.println( lightlevelstr.substring(8,11).toInt() );
      Serial.println("++++++++");

      level_R = lightlevelstr.substring(2,5).toInt();     //ll010010010
      level_G = lightlevelstr.substring(5,8).toInt();     //12345678901
      level_B = lightlevelstr.substring(8,11).toInt();
      Serial.print("RECALL---R G B "); Serial.print(level_R);Serial.print(" ");Serial.print(level_G);Serial.print(" ");Serial.print(level_B);Serial.println(" ");   
      
      BlinkLight(3,150,150,0);
      SetLight(level_R, level_G, level_B);   
      ledEIN=1; 
      Serial.print("RECALL---R G B "); Serial.print(level_R);Serial.print(" ");Serial.print(level_G);Serial.print(" ");Serial.print(level_B);Serial.println(" ");   
  }
}

bool mqtt_reconnect() {
  // Loop until we're reconnected
  int mqttConnectCnt=0;
  bool ReturnVal=false;

  while ( !mqttclient.connected() && (mqttConnectCnt<3) ) 
  {
    mqttConnectCnt++;
    Serial.print(CR "Attempting MQTT connection..<"); Serial.print(mqttConnectCnt); Serial.print("> ");
    // Attempt to connect
    if (  mqttclient.connect("ESP_Client", mqttUser.c_str(), mqttKW.c_str())  ) 
    {
      Serial.println(" mqtt connected");
      mqttConnectCnt=0;
      ReturnVal=true;
      // Subscribe
      //mqttclient.subscribe("LuftCheck/#");
      string myTopic; myTopic.clear(); myTopic.append(mqttTopic.c_str()); myTopic.append("/#");
      mqttclient.subscribe( myTopic.c_str()  );
    } 
    else 
    {
      Serial.print(" mqtt failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
  return ReturnVal; 
}

void mqtt_callback(char* topic, byte* message, unsigned int length) 
{
  Serial.print("mqtt-Message arrived on topic: ");
  Serial.print(topic);
  
  Serial.print(" - Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

   // Um die letzten 4 Buchstaben des Topics zu extrahieren
  String topicString = String(topic);  // Thema als String konvertieren
  String last4 = topicString.substring(topicString.length() - 4);  // Letzte 4 Zeichen extrahieren
  Serial.print(" -- "); Serial.print(last4); Serial.println(" -- "); 

  char *endptr;
  if (last4 == "setR") {
    Serial.print("setR: ");
    unsigned long value = strtoul(messageTemp.c_str(), &endptr, 10);
    Serial.println(value);
    level_R=value;
    SetLight(level_R, level_G, level_B);
  }
  else if (last4 == "setG") {
    Serial.print("setG: ");
    unsigned long value = strtoul(messageTemp.c_str(), &endptr, 10);
    Serial.println(value);
    level_G=value;
    SetLight(level_R, level_G, level_B);
  }
  else if (last4 == "setB") {
    Serial.print("setB: ");
    unsigned long value = strtoul(messageTemp.c_str(), &endptr, 10);
    Serial.println(value);
    level_B=value;
    SetLight(level_R, level_G, level_B);;
  }
}

void publishMQTT(void)
  {
    if ( WiFi.status()  == WL_CONNECTED)
    {
      Serial.println(CR "MQTT publish start: ");
        
      if ( mqtt_reconnect() )
      {
        mqttclient.loop();

        string myTopic;
        char myTopicTXT[7];

        utoa (level_R, myTopicTXT, 10);
        myTopic.clear(); myTopic.append(mqttTopic.c_str()); myTopic.append("/R");
        if ( mqttclient.publish( myTopic.c_str(), myTopicTXT) ) { Serial.println("published R "); }
        
        utoa (level_G, myTopicTXT, 10);
        myTopic.clear(); myTopic.append(mqttTopic.c_str()); myTopic.append("/G");
        if ( mqttclient.publish(myTopic.c_str(), myTopicTXT) ) { Serial.println("published G "); }

        utoa (level_B, myTopicTXT, 10);
        myTopic.clear(); myTopic.append(mqttTopic.c_str()); myTopic.append("/B");
        if ( mqttclient.publish(myTopic.c_str(), myTopicTXT) ) { Serial.println("published B "); }
      }
      else
      { Serial.println( "  --  MQTT publish nicht durchgeführt");}
    }
    else
    {
      Serial.println(CR "NoWifi - No MQTT publish");
    }
  }

int encoderPinALast = LOW;
int encoderPinANow = LOW;
int encoderPos = 0;
 
//Loop ! ------------------------------------------------------------------------------------------------ 
void loop() 
{
  //Encoder Drehung abfragen -----------------------------------------------
  encoderPinANow = digitalRead(ROT_A);
  if ((encoderPinALast == HIGH) && (encoderPinANow == LOW)) {
    if (digitalRead(ROT_B) == HIGH) {
      encoderPos--;
      if (RotEncoder==allColRot)  { SetLEDein(setDUNKEL);}
      else                        { SetLEDein(setRotDUNKEL); }
    } 
    else 
    {
      encoderPos++;
      if (RotEncoder==allColRot)  { SetLEDein(setHELL);}
      else                        { SetLEDein(setRotHELL); }
    }
    Serial.println(encoderPos);

    if (digitalRead(ROT_SW) == LOW)
    {
      Serial.println("KlickUndDreh  DEMO MODE");
      do
      {
        LedStartUp();
        LedStartUp();
        LedStartUp();
        BlinkLight(2, 80, 0, 0);
        delay(2500);
        BlinkLight(2, 0, 80, 0);
        delay(2500);
        BlinkLight(2, 0, 0, 80);
        delay(2500);
        BlinkLight(2, 80, 80, 80);
        delay(2500);
        LedStartUp();
        LedStartUp();
        delay(1500);
        SetLight(25, 25, 0);
        delay(5500);
        SetLight(25, 25, 25);
        delay(5500);
        SetLight(0, 25, 25);
        delay(5500);
        SetLight(20, 25, 25);
        delay(5500);
        SetLight(20, 0, 25);
        delay(5500);
      } while (digitalRead(ROT_SW) == HIGH);
    }

  } 
  
  encoderPinALast = encoderPinANow;

    // Encoder Click abfragen -----------------------------------------------
    if (digitalRead(ROT_SW) == LOW)
    {
      /* code */
      delay(50); // Mini Entprellung

      int ClickTime = 0;
      Click = kurzClick;
      while ((digitalRead(ROT_SW) == LOW) && (ClickTime < 40))
      {
        ClickTime++;
        Serial.print(".");
        delay(30);
      }

      if (ClickTime > 8)
      {
        Click = langClick;
      }
      
      delay(50); // Mini Entprellung
      Serial.print("-exit Click-");
  }

  if (Click == kurzClick)
  {
    Serial.println("kurzClick");
    SetLEDein(setTOGGLE);
    if ( DoMQTT ) { publishMQTT(); }
    Click = keinClick;
  }

  if (Click == langClick)
  {
    Serial.println("langClick");
    ++RotEncoder;     //enum RotArt {rRot, gRot, bRot, allColRot};
    Serial.printf("RotArt: %i" CR,RotEncoder);
    
    switch (RotEncoder)
    {
    case rRot: 
        BlinkLight(2,150,0,0);
        break;
    case gRot: 
        BlinkLight(2,0,150,0);
        break;
    case bRot: 
        BlinkLight(2,0,0,150);
        break;
    case allColRot: 
        BlinkLight(2,150,150,150);
        break;
    default: 
        Serial.println("Hier sollte man nie sein!");
        break;
    }
    
    Click = keinClick;
  }


  //Webserver abfragen -----------------------------------------------
  if (WEBcommand == ein) {
    Serial.println("EIN_WEB");
    SetLEDein(setEIN);
    WEBcommand = "";
    if ( DoMQTT ) { publishMQTT(); }
  } // if
   
  if (WEBcommand == aus) {
    Serial.println("AUS_WEB");
    SetLEDein(setAUS);
    WEBcommand = "";
    if ( DoMQTT ) { publishMQTT(); }
  } // if
        
  if (WEBcommand == heller) {
    Serial.println("HELLER_WEB");
    SetLEDein(setHELL);
    WEBcommand = "";
    if ( DoMQTT ) { publishMQTT(); }
  } // if
        
  if (WEBcommand == dunkler) {
    Serial.println("DUNKLER_WEB");
    SetLEDein(setDUNKEL);
    WEBcommand = "";
    if ( DoMQTT ) { publishMQTT(); }
  } // if

  if (WEBcommand == save) {
    Serial.println("SAVE_WEB");
    SetLEDein(setSAVE);
    WEBcommand = "";
  } // if
  
  if (WEBcommand == recall) {
    Serial.println("RECALL_WEB");
    SetLEDein(setRECALL);
    WEBcommand = "";
  }  // if
   
  if (WEBcommand == rgb) { 
    hexstring=rgb_werte;
    Serial.print("RGB HEX:"); Serial.println(hexstring);

    level_R = (int) strtol( hexstring.substring(1,3).c_str(), NULL, 16);
    level_G = (int) strtol( hexstring.substring(3,5).c_str(), NULL, 16);
    level_B = (int) strtol( hexstring.substring(5,7).c_str(), NULL, 16);
    
    Serial.print("RGB von Web: ");
    Serial.print(level_R); Serial.print(" / "); Serial.print(level_G); Serial.print(" / "); Serial.println(level_B);
              
    // kurz blinken und Farbe anzeigen
    BlinkLight(3, 200,200,200);
    SetLight(level_R, level_G, level_B);
    ledEIN=1;
    WEBcommand = "";
    if ( DoMQTT ) { publishMQTT(); }
  } // if
          
  yield();

  //MQTT==============================================================
    if (( WiFi.status()  == WL_CONNECTED) &&(mqttIntervall > 0) ) { mqttclient.loop(); }

    if (mqttIntervall > 0)    //mqttIntervall 0 : kein mqtt
    {
      currentMillis = millis();
      if ( (currentMillis - previousMillisPublMQTT) >= (interval1min * mqttIntervall  ) )  
      {  
        previousMillisPublMQTT=currentMillis;
        publishMQTT();
      }
    } 
  

  //Zum Testen der Farben, keine Funktion der Gestenlampe
  if (Serial.available())
  {
    char Zeichen = (char)Serial.read();
    Serial.print(">>"); Serial.println(Zeichen);
    if (Zeichen=='0')
    {   Serial.println("Zeichen 0 empfangen"); 
        level_R = 0;
        level_G = 0;
        level_B = 0;
        SetLight(level_R, level_G, level_B);
    }
    if (Zeichen=='r')
    {   Serial.println("Zeichen r empfangen"); 
        level_R = 255;
        level_G = 0;
        level_B = 0;
        SetLight(level_R, level_G, level_B);
    }
    if (Zeichen=='g')
    {   Serial.println("Zeichen g empfangen"); 
        level_R = 00;
        level_G = 255;
        level_B = 0;
        SetLight(level_R, level_G, level_B);
    }
    if (Zeichen=='b')
    {   Serial.println("Zeichen b empfangen"); 
        level_R = 0;
        level_G = 0;
        level_B = 255;
        SetLight(level_R, level_G, level_B);
    }
    if (Zeichen=='w')
    {   Serial.println("Zeichen w empfangen"); 
        level_R = 255;
        level_G = 255;
        level_B = 255;
        SetLight(level_R, level_G, level_B);
    }
    if (Zeichen=='p')
    {   Serial.println("publishMQTT"); 
        publishMQTT();
    }
    
  }

  //WIFI Verbindung prüfen
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >=interval10min)
  { 
    previousMillis = currentMillis; 
    if (myWifi.connectToWifi() != WL_CONNECTED)
      {
        myWifi.connectToAP();
      }
  }

  yield();

} // loop
  
    
//= Webseiten ====================================================================


void handleRoot(AsyncWebServerRequest *request)
 {                  //Html Startseite

   Serial.printf("##WEBPAGE handleRoot Parameter: %i \n", request->params());

    String inputMessage;
    String Parameter;

    WEBcommand="";
    rgb_werte="";
     
    /*
    int paramsNr = request->params();
    Serial.println(paramsNr);
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");   Serial.println(p->name());
        Serial.print("Param value: ");  Serial.println(p->value());
    }
    */

    Parameter="rgb";
    if (request->hasParam(Parameter)) 
    {
       WEBcommand = request->getParam(Parameter)->name();
       rgb_werte = request->getParam(Parameter)->value();
       Serial.print("rgb_werte  >>");
       Serial.print(rgb_werte);
       Serial.println("<<");
    }

    Parameter="ledon";
    if (request->hasParam(Parameter))  {  WEBcommand = request->getParam(Parameter)->value();   }

    Parameter="ledoff";
    if (request->hasParam(Parameter))  {  WEBcommand = request->getParam(Parameter)->value();   }

    Parameter="heller";
    if (request->hasParam(Parameter))  {  WEBcommand = request->getParam(Parameter)->value();   }

    Parameter="dunkler";
    if (request->hasParam(Parameter))  {  WEBcommand = request->getParam(Parameter)->value();   }

    Parameter="save";
    if (request->hasParam(Parameter))  {  WEBcommand = request->getParam(Parameter)->value();   }

    Parameter="recall";
    if (request->hasParam(Parameter))  {  WEBcommand = request->getParam(Parameter)->value();   }

    Serial.print("WEBcommand: "); Serial.print(WEBcommand); Serial.print(">>"); Serial.println(rgb_werte);
    request->send_P(200, "text/html", index_html); 
}

bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, String &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        myValue=inputMessage; myValue.trim();
        preferences.putString(Parametername.c_str(), myValue );   
        Serial.print("preferences.putString " + Parametername + ": "); Serial.println(myValue);
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}

bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, float &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        myValue=atof(inputMessage.c_str());
        preferences.putFloat(Parametername.c_str(),  myValue );   
        Serial.print("preferences.putFloat " + Parametername + ": "); Serial.println(inputMessage );
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}

bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, int &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        myValue=atoi(inputMessage.c_str());
        preferences.putInt(Parametername.c_str(),  myValue );   
        Serial.print("preferences.putInt " + Parametername + ": "); Serial.println(inputMessage );
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}

bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, bool &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        if (inputMessage=="ein") {myValue=true;}
        if (inputMessage=="aus") {myValue=false;}
        
        preferences.putInt(Parametername.c_str(),  myValue );   
        Serial.print("preferences.putInt(bool) " + Parametername + ": "); Serial.println(myValue );
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}


void handleSetup(AsyncWebServerRequest *request) 
  {
    Serial.println("##WEBPAGE handlesetCalValues");
    
    //List all parameters (Compatibility)
    /*
    int args = request->args();
    for(int i=0;i<args;i++){
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
    */

    preferences.begin("calibration", false);
    GetAndWriteParameter(request, "espBeschreibung", espBeschreibung);
    GetAndWriteParameter(request, "mqttLink", mqttLink);
    GetAndWriteParameter(request, "mqttUser", mqttUser);
    GetAndWriteParameter(request, "mqttKW", mqttKW);
    GetAndWriteParameter(request, "mqttTopic", mqttTopic);
    GetAndWriteParameter(request, "mqttIntervall", mqttIntervall);
    preferences.end(); 
    Serial.println("WERTE GESCHRIEBEN");
    yield(); 
    String  HtmlPage="<html><meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'> \n";
            HtmlPage+="<style>html {font-family: Arial; display: inline-block; text-align: center;}</style> \n";
            HtmlPage+="<body><br><h4>Einstellungen gespeichert! <br> <h3><a href='/'>LuftCheck</a></body></html>";
    request->send(200, "text/html", HtmlPage);
  }



void handleledStatus(AsyncWebServerRequest *request) {

Serial.println("##WEBPAGE handleledStatus");

  if(ledEIN==1){
    request->send_P(200, "text/plain", "EIN" );
  }
  else {
    request->send_P(200, "text/plain", "AUS" );
  }
}

void handlenotFound(AsyncWebServerRequest *request) {
  Serial.println("##WEBPAGE handlenotFound");
  request->send(404, "text/plain", "404 - Not found on my ESP");
}

// Replaces placeholder with button section in your web page
String processor(const String& var)
{
  Serial.print("page processor >>"); Serial.println(var);
  String returnTXT = "\n";
  
  if(var == "mqttLink"){ returnTXT = mqttLink;  }
  if(var == "mqttUser"){ returnTXT = mqttUser;  }
  if(var == "mqttKW"){ returnTXT = mqttKW;  }
  if(var == "mqttTopic"){ returnTXT = mqttTopic;  }
  if(var == "mqttIntervall"){ returnTXT = String(mqttIntervall); }
  
  if(var == "espHostname"){ returnTXT = espHostname;  }
  if(var == "espBeschreibung"){ returnTXT = espBeschreibung;  }

  return returnTXT;
}

  

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void LedStartUp(void)
{
  int mydelay=3;
  
  Serial.println("R increase the LED brightness");
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(ledChannelR, dutyCycle); delay(mydelay);
  }
  Serial.println("R decrease the LED brightness");
  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(ledChannelR, dutyCycle);  delay(mydelay);
  }
  Serial.println("G increase the LED brightness");
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(ledChannelG, dutyCycle);  delay(mydelay);
  }
  Serial.println("G decrease the LED brightness");
  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(ledChannelG, dutyCycle);  delay(mydelay);
  }
  Serial.println("B increase the LED brightness");
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
    ledcWrite(ledChannelB, dutyCycle);  delay(mydelay);
  }
  Serial.println("B decrease the LED brightness");
  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(ledChannelB, dutyCycle); delay(mydelay);
  }
}


