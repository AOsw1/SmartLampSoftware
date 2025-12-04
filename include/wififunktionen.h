#ifndef WIFIFUNKTIONEN_H
#define WIFIFUNKTIONEN_H
 
    extern String ssid[3];
    extern String password[3];
    extern String espHostname;

    String readFile(fs::FS &fs, const char * path);
    void writeFile(fs::FS &fs, const char * path, const char * message);
    
    ushort connectToWifi();
    
    void handleSetupWIFI(AsyncWebServerRequest *request);
    void handlegetSetupWIFI(AsyncWebServerRequest *request);

    String readFile(fs::FS &fs, const char * path);
    void writeFile(fs::FS &fs, const char * path, const char * message);
    extern String ssid[3];
    extern String password[3];

    class WifiFunktionen 
    {
        public:
        ushort connectToWifi();
        ushort connectToAP();
        void WifiOFF();
    };


#endif // WIFIFUNKTIONEN_H