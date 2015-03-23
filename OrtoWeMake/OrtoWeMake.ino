/* Orto WeMake Arduino Day 2015 */

#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>

#define textBuffSize 500
#define PhMeterPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset -0.26             //deviation compensate
#define ArrayLenth  40           //times of collection


byte pinLamp[10]  = {22,23,24,25,26,27,28,29,10,31};
byte pinSenEC[10] = {44,45,46,47,48,49,50,51,52,53};
byte pinPump1     = 2;
byte pinFert1     = 3;

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
const char*  SANEC2= "5509b3d74ca0f61a7c9ced83"; // Sensore EC 2  (Tropaeolum majus) 
const char*  SENEC3= "5509b3d74ca0f61a7c9ced84"; // Sensore EC 3  (Ocimum basilicum)
const char*  SENEC4= "5509b3d74ca0f61a7c9ced85"; // Sensore EC 4  (Lactuca sativa)
const char*  SENEC5= "5509b3d74ca0f61a7c9ced86"; // Sensore EC 5  (Lactuca sativa var. foglia d quercia)
const char*  SENEC6= "5509b3d74ca0f61a7c9ced87"; // Sensore EC 6  (Tagetes sp)
const char*  SENEC7= "5509b3d74ca0f61a7c9ced88"; // Sensore EC 7  (Eruca vesicaria)
const char*  SENEC8= "5509b3d74ca0f61a7c9ced89"; // Sensore EC 8  (Spinacia oleracea)
const char*  SENEC9= "5509b3d74ca0f61a7c9ced8a"; // Sensore EC 9  (Melissa officinalis)
const char*  SENEC0= "5509b3d74ca0f61a7c9ced8b"; // Sensore EC 10 (Capsicum)
const char*  BATTE1= "5509b3d74ca0f61a7c9ced77"; // Voltaggio batteria (SmartCitizen)  

unsigned int localPort = 8888;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//char server_in[] = "api.smartcitizen.me";    // name address for Google (using DNS)
//char server_out[] = "iot.enter.it";    // name address for Google (using DNS)
IPAddress server_in(104,28,14,99); // api.smartcitizen.me
IPAddress server_out(185,48,32,120); // iot.enter.it
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server

const int NTP_PACKET_SIZE= 48;
byte packetBuffer[ NTP_PACKET_SIZE];

int index;
char results[textBuffSize];
boolean startData = false;
char* strBuffer;

//const char* timestamp;
double temp, hum, co, no2, noise, light, batt;
  
IPAddress ip(192, 168, 2, 249);
EthernetClient client;
EthernetUDP Udp;

char currHour[2], currMin[2], currSec[2];
static float pHValue = 0;

boolean statusLight = false;

/**********************************************************************************/

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; } // wait for serial port to connect. Needed for Leonardo only
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  
  Udp.begin(localPort);
  
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
  getTime();
  Serial.print(currHour);Serial.print(":");Serial.print(currMin);Serial.print(":");Serial.println(currSec);
  
  for (byte i=0; i < 10; i++) { pinMode(pinLamp[i],OUTPUT); pinMode(pinSenEC[i],OUTPUT); }
}

/**********************************************************************************/

void loop() {
   if (currMin == "00" || currMin == "10" || currMin == "20" || currMin == "30" || currMin == "40" || currMin == "50" ) { getTime(); delay( 1000 ); }
   
   if (currMin == "00" || currMin == "30" ) {
     getApiSmartCitizen();  // Get Data from Smart Citizen project
     if ( results[0] == '{' ) {
      Serial.print("temp "); Serial.println(temp);
      Serial.print("hum "); Serial.println(hum);
      Serial.print("co "); Serial.println(co);
      Serial.print("no2 "); Serial.println(no2);
      Serial.print("noise "); Serial.println(noise);
      Serial.print("light "); Serial.println(light);
      Serial.print("batt "); Serial.println(batt);
      
      strBuffer=""; sprintf(strBuffer, "%f", temp);  postData(TEMPC1, strBuffer);
      strBuffer=""; sprintf(strBuffer, "%f", hum);   postData(HUMID1, strBuffer);
      strBuffer=""; sprintf(strBuffer, "%f", co);    postData(CO2SN1, strBuffer);
      strBuffer=""; sprintf(strBuffer, "%f", no2);   postData(NO2SN1, strBuffer);
      strBuffer=""; sprintf(strBuffer, "%f", noise); postData(NOISE1, strBuffer);
      strBuffer=""; sprintf(strBuffer, "%f", light); postData(LIAMB1, strBuffer);
      strBuffer=""; sprintf(strBuffer, "%f", batt);  postData(BATTE1, strBuffer);
      
     }
     
     getPh(); if (pHValue > 0 ) { strBuffer=""; sprintf(strBuffer, "%f", pHValue);  postData(PHMET1, strBuffer); }
   }
   
   if (currHour == "07" && statusLight == false ) { // Luci accese
     for (byte i=0; i < 10; i++) { digitalWrite(pinLamp[i],HIGH); }
     postData(LIGHT1, "1");
     postData(LIGHT2, "1");
     postData(LIGHT3, "1");
     postData(LIGHT4, "1");
     postData(LIGHT5, "1");
     postData(LIGHT6, "1");
     postData(LIGHT7, "1");
     postData(LIGHT8, "1");
     postData(LIGHT9, "1");
     postData(LIGHT0, "1");
     statusLight = true;
   }
   
   if (currHour == "21" && statusLight == true ) { // Luci accese
     for (byte i=0; i < 10; i++) { digitalWrite(pinLamp[i],LOW); }
     postData(LIGHT1, "0");
     postData(LIGHT2, "0");
     postData(LIGHT3, "0");
     postData(LIGHT4, "0");
     postData(LIGHT5, "0");
     postData(LIGHT6, "0");
     postData(LIGHT7, "0");
     postData(LIGHT8, "0");
     postData(LIGHT9, "0");
     postData(LIGHT0, "0");
     statusLight = false;
   }
   
   delay( 60000 );
}

