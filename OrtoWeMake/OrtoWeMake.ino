/* Orto WeMake Arduino Day 2015 */

#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>

/**********************************************************************************/

#define DEBUG

/**********************************************************************************/

#define textBuffSize 500
#define PhMeterPin   A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset       -0.26         //deviation compensate
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
double temp, hum, co, no2, light;
int noise, batt;
  
IPAddress ip(192, 168, 2, 249);
EthernetClient client;
EthernetUDP Udp;

byte currHour, currMin, currSec;
static float pHValue = 0;

/**********************************************************************************/

boolean statusLight = false;

/**********************************************************************************/

byte nsens = 2;
byte pinEC1[] = {2,8};
byte pinEC2[] = {0,1};
byte pinEC3[] = {3,9};
// parametri di taratura in uS/cm
float A[] = {870.43,822.71};
float B[] = {-1.194,-1.190};
float R1 = 470.;  //  resistenza fissa tra P1 e P3 in ohm
float RS[] = {0,0}; //  inizializzo resistenza tra i due elettrodi 
float CS[] = {0,0}; //  inizializzo conducibilità specifica

byte isens = 0;
byte n = 50;

/**********************************************************************************/

const float ECValRif = 0.9;

/**********************************************************************************/

void setup() {
  Serial.begin(9600);
  
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
  
  pinMode(pinLamp1,OUTPUT);
  pinMode(pinLamp1,OUTPUT);
  pinMode(pinPump1,OUTPUT);
  pinMode(pinPump2,OUTPUT);
  pinMode(pinFert1,OUTPUT);
  
  pinMode(pinValF1,INPUT);
  
  for(byte isens = 0; isens < nsens ; isens++){
    pinMode(pinEC1[isens],OUTPUT);
    pinMode(pinEC1[isens],INPUT);
    pinMode(pinEC3[isens],OUTPUT);
  }
  
  analogWrite(pinFert1,0);
  
}

/**********************************************************************************/
byte lastMin = 0;

void loop() {
   /**********************************************************************************/

   if ( (lastMin+20) == currMin ) { 
      getTime(); 
      lastMin = currMin;
      delay( 1000 );
   }
   
   /**********************************************************************************/

   if ((0 < currMin && currMin < 59) || ( 30 < currMin && currMin < 35 )) {
     while ( !getApiSmartCitizen() ) { ; }  // Get Data from Smart Citizen project
     if ( results[0] == '{' ) {

      #ifdef DEBUG
        Serial.print("temp "); Serial.println(temp);
        Serial.print("hum "); Serial.println(hum);
        Serial.print("co "); Serial.println(co);
        Serial.print("no2 "); Serial.println(no2);
        Serial.print("noise "); Serial.println(noise);
        Serial.print("light "); Serial.println(light);
        Serial.print("batt "); Serial.println(batt);
      #endif
      
      // Mapping Value
      co = map(((int)((co/100) + 0.5)*100),1,1000,300,1500);
      no2 = map(((int)(no2/100 + 0.5)*100),0,500,0,1);
      noise = map((((int)(noise/10)+0.5)*10),0,100,10,100);
      batt = map(batt,0,100,0,5);
      
      #ifdef DEBUG2
        Serial.print("temp "); Serial.println(temp);
        Serial.print("hum "); Serial.println(hum);
        Serial.print("co "); Serial.println(co);
        Serial.print("no2 "); Serial.println(no2);
        Serial.print("noise "); Serial.println(noise);
        Serial.print("light "); Serial.println(light);
        Serial.print("batt "); Serial.println(batt);
      #endif
      
      strBuffer=""; dtostrf(temp,2,0,strBuffer);  while ( !postData(TEMPC1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(hum,2,0,strBuffer);   while ( !postData(HUMID1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(co,4,0,strBuffer);    while ( !postData(CO2SN1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(no2,1,0,strBuffer);   while ( !postData(NO2SN1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(noise,3,0,strBuffer); while ( !postData(NOISE1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(light,2,0,strBuffer); while ( !postData(LIAMB1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(batt,2,0,strBuffer);  while ( !postData(BATTE1, strBuffer) ) { ; }
     }

     /**********************************************************************************
     
     getPh();
     if (pHValue > 0 ) { 
        strBuffer="";
        dtostrf(pHValue,2,2,strBuffer);
        while ( !postData(PHMET1, strBuffer) ) { ; }
      }
     /**********************************************************************************/

     float CS1 = readEC(0);
     if (CS1 > 0 && CS1 < 1.2) { 
       strBuffer=""; dtostrf(CS1,1,1,strBuffer);  
       
       while ( !postData(SENEC1, strBuffer) ) { ; }
       while ( !postData(SENEC2, strBuffer) ) { ; }
       while ( !postData(SENEC3, strBuffer) ) { ; }
       while ( !postData(SENEC4, strBuffer) ) { ; }
       while ( !postData(SENEC5, strBuffer) ) { ; }
     }
     
     float CS2 = readEC(1);
     if (CS2 > 0 && CS2 < 1.2) { 
       strBuffer=""; dtostrf(CS2,1,1,strBuffer);  
       
       while ( !postData(SENEC6, strBuffer) ) { ; }
       while ( !postData(SENEC7, strBuffer) ) { ; }
       while ( !postData(SENEC8, strBuffer) ) { ; }
       while ( !postData(SENEC9, strBuffer) ) { ; }
       while ( !postData(SENEC0, strBuffer) ) { ; }
     }
     
     /**********************************************************************************/

  
   } // end if 0 and 30 min
   
   /**********************************************************************************/
     
   // FERTILIZZANTE
   if ( currHour == 22 && (30 < currMin && currMin < 31)) {
     apriGocciolatore();
     while ( !postData(FERTI1, "1") ) { ; }
     delay( 20000 );
     chiudiGocciolatore();
     while ( !postData(FERTI1, "0") ) { ; }
   }

   /**********************************************************************************/

   if ( 7 <= currHour && currHour <= 20 && statusLight == false ) { // Luci accese
     digitalWrite(pinLamp1,HIGH);
     digitalWrite(pinLamp2,HIGH);
     
     while ( !postData(LIGHT1, "1") ) { ; }
     while ( !postData(LIGHT2, "1") ) { ; }
     while ( !postData(LIGHT3, "1") ) { ; }
     while ( !postData(LIGHT4, "1") ) { ; }
     while ( !postData(LIGHT5, "1") ) { ; }
     while ( !postData(LIGHT6, "1") ) { ; }
     while ( !postData(LIGHT7, "1") ) { ; }
     while ( !postData(LIGHT8, "1") ) { ; }
     while ( !postData(LIGHT9, "1") ) { ; }
     while ( !postData(LIGHT0, "1") ) { ; }
     
     statusLight = true;
   }
   
   if ( ((21 <= currHour && currHour <= 23)  || (0 <= currHour && currHour <= 6)) && statusLight == true ) { // Luci accese
     digitalWrite(pinLamp1,LOW);
     digitalWrite(pinLamp2,LOW);
     
     while ( !postData(LIGHT1, "0") ) { ; }
     while ( !postData(LIGHT2, "0") ) { ; }
     while ( !postData(LIGHT3, "0") ) { ; }
     while ( !postData(LIGHT4, "0") ) { ; }
     while ( !postData(LIGHT5, "0") ) { ; }
     while ( !postData(LIGHT6, "0") ) { ; }
     while ( !postData(LIGHT7, "0") ) { ; }
     while ( !postData(LIGHT8, "0") ) { ; }
     while ( !postData(LIGHT9, "0") ) { ; }
     while ( !postData(LIGHT0, "0") ) { ; }
     
     statusLight = false;
   }
   
   /**********************************************************************************/
   
   if ( (currHour == 8 || currHour == 13 || currHour == 17 || currHour == 20 ) && (0 < currMin && currMin < 6) ) {  // Sei in orario di irrigazione
      digitalWrite(pinPump1,HIGH);
      while ( !postData(POMPA1, "1") ) { ; }
   } else {
      digitalWrite(pinPump1,LOW);
      while ( !postData(POMPA1, "0") ) { ; }
   }
   
   /**********************************************************************************/
   
   delay( 60000 );
} // end loop
