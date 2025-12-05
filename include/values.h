/** 
 *  Definitionen
 */


// Ein Zeilenumbruch, abhängig davon, was die Gegenstelle haben will 
// Windows: "rn" 
// Linux  : "n" 
// MacOS  : "r" 
#define CR "\r\n"

#define SWver "v1.1 202512"

// Pins
#define ROT_A    21    // Needs to be an interrupt pin 
#define ROT_B    19    
#define ROT_SW    5   
#define LED_PIN_NANO_G    27  
#define LED_PIN_NANO_R    26
#define LED_PIN_NANO_B    25

  // setting PWM properties
  const int freq = 5000;
  const int ledChannelR = 0;
  const int ledChannelG = 1;
  const int ledChannelB = 2;
  const int resolution = 8;

  enum setCommand {setEIN=1, setAUS, setHELL, setDUNKEL, setTOGGLE, setSAVE, setRECALL, setRotHELL, setRotDUNKEL};

