/**********************************************************************************/

#define DEBUG 1

/**********************************************************************************/

#define textBuffSize 500
#define PhMeterPin   8             //pH meter Analog output to Arduino Analog Input 0
#define Offset       0.24          //deviation compensate
#define ArrayLenth   40            //times of collection
#define pinLamp1     22            // Relay right Light
#define pinLamp2     23            // Relay left Light
#define pinPump1     32            // Relay Pump 1
#define pinPump2     33            // Relay Pump 2
#define pinFert1     3             // elettrovalvola Fertilizzante
#define pinValF1     2             // controllo valvola Fertilizzante

/**********************************************************************************/

const char*  LIGHT1= "5509b3d74ca0f61a7c9ced62"; // Accensione Luci  1 (Fragaria vesca) 
const char*  LIGHT2= "5509b3d74ca0f61a7c9ced63"; // Accensione Luci  2 (Tropaeolum majus)
const char*  LIGHT3= "5509b3d74ca0f61a7c9ced64"; // Accensione Luci  3 (Ocimum basilicum)
const char*  LIGHT4= "5509b3d74ca0f61a7c9ced65"; // Accensione Luci  4 (Lactuca sativa)
const char*  LIGHT5= "5509b3d74ca0f61a7c9ced66"; // Accensione Luci  5 (Lactuca sativa var. foglia d quercia)
const char*  LIGHT6= "5509b3d74ca0f61a7c9ced67"; // Accensione Luci  6 (Tagetes sp)
const char*  LIGHT7= "5509b3d74ca0f61a7c9ced68"; // Accensione Luci  7 (Eruca vesicaria)
const char*  LIGHT8= "5509b3d74ca0f61a7c9ced69"; // Accensione Luci  8 (Spinacia oleracea)
const char*  LIGHT9= "5509b3d74ca0f61a7c9ced6a"; // Accensione Luci  9 (Melissa officinalis)
const char*  LIGHT0= "5509b3d74ca0f61a7c9ced6b"; // Accensione Luci 10 (Capsicum)
const char*  POMPA1= "5509b3d74ca0f61a7c9ced6c"; // Pompa per parete verticale
const char*  FERTI1= "5509b3d74ca0f61a7c9ced6d"; // Fertilizzante
const char*  ACLEV1= "5509b3d74ca0f61a7c9ced6e"; // Livello acqua bidone idroponica
const char*  NO2SN1= "5509b3d74ca0f61a7c9ced6f"; // Sensore NO2 (SmartCitizen)
const char*  NOISE1= "5509b3d74ca0f61a7c9ced72"; // Rumore ambiente (SmartCitizen)
const char*  TEMPC1= "5509b3d74ca0f61a7c9ced75"; // Temperatura ambiente (SmartCitizen)
const char*  PHMET1= "5509b3d74ca0f61a7c9ced76"; // Ph Idroponica
const char*  CO2SN1= "5509b3d74ca0f61a7c9ced71"; // CO2 ambiente (SmartCitizen)
const char*  LIAMB1= "5509b3d74ca0f61a7c9ced73"; // Luce ambiente (SmartCitizen)
const char*  HUMID1= "5509b3d74ca0f61a7c9ced74"; // Sensore umidità (SmartCitizen)
const char*  SENEC1= "5509b3d74ca0f61a7c9ced82"; // Sensore EC 1  (Fragaria vesca)
const char*  SENEC2= "5509b3d74ca0f61a7c9ced83"; // Sensore EC 2  (Tropaeolum majus) 
const char*  SENEC3= "5509b3d74ca0f61a7c9ced84"; // Sensore EC 3  (Ocimum basilicum)
const char*  SENEC4= "5509b3d74ca0f61a7c9ced85"; // Sensore EC 4  (Lactuca sativa)
const char*  SENEC5= "5509b3d74ca0f61a7c9ced86"; // Sensore EC 5  (Lactuca sativa var. foglia d quercia)
const char*  SENEC6= "5509b3d74ca0f61a7c9ced87"; // Sensore EC 6  (Tagetes sp)
const char*  SENEC7= "5509b3d74ca0f61a7c9ced88"; // Sensore EC 7  (Eruca vesicaria)
const char*  SENEC8= "5509b3d74ca0f61a7c9ced89"; // Sensore EC 8  (Spinacia oleracea)
const char*  SENEC9= "5509b3d74ca0f61a7c9ced8a"; // Sensore EC 9  (Melissa officinalis)
const char*  SENEC0= "5509b3d74ca0f61a7c9ced8b"; // Sensore EC 10 (Capsicum)
const char*  BATTE1= "5509b3d74ca0f61a7c9ced77"; // Voltaggio batteria (SmartCitizen)  

/**********************************************************************************/

unsigned int localPort = 8888;
byte timeZone = 1;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server_in[]  = "api.smartcitizen.me";    // name address for Google (using DNS)
char server_out[] = "iot.enter.it";    // name address for Google (using DNS)
char timeServer[] = "time.nist.gov";     // NTP Server

const int NTP_PACKET_SIZE= 48;
byte packetBuffer[ NTP_PACKET_SIZE];

int index;
char results[textBuffSize];
boolean startData = false;
char* strBuffer;

//const char* timestamp;
double temp, hum, co, no2, light;
int noise, batt;
  
IPAddress ip(192, 168, 2, 249);
EthernetClient client;
EthernetUDP Udp;

byte currHour, currMin, currSec;
static float pHValue = 0;

/**********************************************************************************/

boolean statusLight = false;
boolean statusPompa = false;
const float ECValRif = 0.9;
byte lastTimeSet = 0;
char lcdBuffer1[16] = "";
char lcdBuffer2[16] = "";

/**********************************************************************************/

byte nsens = 2;
byte pinEC1[] = {51,50};
byte pinEC2[] = {15,14};
byte pinEC3[] = {53,52};
// parametri di taratura in uS/cm
float A[] = {870.43,822.71};
float B[] = {-1.194,-1.190};
float R1 = 470.;  //  resistenza fissa tra P1 e P3 in ohm
float RS[] = {0,0}; //  inizializzo resistenza tra i due elettrodi 
float CS[] = {0,0}; //  inizializzo conducibilità specifica

byte isens = 0;
byte n = 50;

