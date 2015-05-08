/* Orto WeMake Arduino Day 2015 */

#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <EthernetUdp.h>
#include <Wire.h>
#include "RTClib.h"
#include "ortoConfig.h"
#include <avr/wdt.h>

/**********************************************************************************/

RTC_DS1307 RTC;

/**********************************************************************************/

void setup() {
  Serial.begin(9600);
  for (int t=0; t<8000; t++) { delay(1); }
  
  wdt_disable();
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  
  Udp.begin(localPort);
  Wire.begin();
  RTC.begin();
  
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println("");
  
  if (! RTC.isrunning()) { Serial.println("RTC NOT running! RESET REQUIRED"); }
  else                   { RTC.adjust(DateTime(__DATE__, __TIME__)); }
  
  DateTime now = RTC.now();
  getTime(); 
  RTC.adjust(DateTime(now.year(), now.month(), now.day(), currHour, currMin, currSec)); 
  // lastTimeSet = currHour;
  
  pinMode(pinLamp1,OUTPUT);
  pinMode(pinLamp2,OUTPUT);
  pinMode(pinPump1,OUTPUT);
  pinMode(pinPump2,OUTPUT);
  pinMode(pinFert1,OUTPUT);
  
  pinMode(pinValF1,INPUT);
  
  for(byte isens = 0; isens < nsens ; isens++){
    pinMode(pinEC1[isens],OUTPUT);
    pinMode(pinEC2[isens],INPUT);
    pinMode(pinEC3[isens],OUTPUT);
  }
  analogWrite(pinFert1,0);
}

/**********************************************************************************/

void loop() {
  DateTime now = RTC.now();
  Serial.println("\n---------------");
  
  sprintf(lcdBuffer2,  "Ora: %02d:%02d:%02d DayOfWeek: %d", (currHour+LegalTime(RTC.now())), currMin, currSec, now.dayOfWeek()); 
  Serial.println(lcdBuffer2);
  
  /**********************************************************************************/
  
  if ((currHour == 9 || currHour == 13 || currHour == 19) && currMin == 8) { 
    delay( 30000 ); 
    wdt_enable(WDTO_8S); 
    delay( 10000 );
  }
  
  /**********************************************************************************/
  
  if ( currHour == 2 && currMin == 15) { 
      getTime(); 
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), currHour, currMin, currSec)); 
      lastTimeSet = currHour;
   } else {
     currHour = now.hour();
     currMin  = now.minute();
     currSec  = now.second();
   }
   
   /**********************************************************************************/
   
   if ( 0 == currMin || currMin == 30 ) { // Get Data from Smart Citizen project
     while ( !getApiSmartCitizen() ) { ; }
     if ( results[0] == '{' ) {

      // Mapping Value
      co = map(((int)((co/100) + 0.5)*100),1,1000,300,1500);
      no2 = (no2/500);
      noise = map((((int)(noise/10)+0.5)*10),0,100,10,100);
      batt = map(batt,0,100,0,5);

      /*
      Serial.print( "Temp: " ); Serial.print(temp);  Serial.println(" ");
      Serial.print( "Hum:  " ); Serial.print(hum);   Serial.println(" %");
      Serial.print( "Co: "   ); Serial.print(co);    Serial.println(" Co");
      Serial.print( "No2: " );  Serial.print(no2);   Serial.println(" No");
      Serial.print( "Nois: " ); Serial.print(noise); Serial.println("");
      Serial.print( "Luce: " ); Serial.print(light); Serial.println("");
      Serial.print( "Batt: " ); Serial.print(batt);  Serial.println("v");
      */
      
      strBuffer=""; dtostrf(temp,2,0,strBuffer);  while ( !postData(TEMPC1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(hum,2,0,strBuffer);   while ( !postData(HUMID1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(co,4,0,strBuffer);    while ( !postData(CO2SN1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(no2,1,0,strBuffer);   while ( !postData(NO2SN1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(noise,3,0,strBuffer); while ( !postData(NOISE1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(light,2,0,strBuffer); while ( !postData(LIAMB1, strBuffer) ) { ; }
      strBuffer=""; dtostrf(batt,2,0,strBuffer);  while ( !postData(BATTE1, strBuffer) ) { ; }
     }

     /**********************************************************************************/
     
     getPh();
     if (pHValue > 0 ) { 
        strBuffer=""; dtostrf(pHValue,2,2,strBuffer);
        while ( !postData(PHMET1, strBuffer) ) { ; }
      }
     
     /**********************************************************************************/
  
     digitalWrite(pinEC1[1],HIGH);
     if ( analogRead(pinEC2[1]) > 512 ) { while ( !postData(ACLEV1, "0") ) { ; } }
     else                               { while ( !postData(ACLEV1, "1") ) { ; } } 
     digitalWrite(pinEC1[1],LOW);
     
     /**********************************************************************************/
  
   } // end if 0 and 30 min
   
  /**********************************************************************************/

  if ( ((20 <= currHour && currHour <= 23)  || (5 <= currHour && currHour <= 7)) && currMin == 20 ) {
     float CS1 = readEC(0);
     if (CS1 < 0)   { CS1 = 0; }
     if (CS1 > 1.3) { CS1 = 1.3; } 
     strBuffer=""; dtostrf(CS1,1,1,strBuffer);  
       
     while ( !postData(SENEC1, strBuffer) ) { ; }
     while ( !postData(SENEC2, strBuffer) ) { ; }
     while ( !postData(SENEC3, strBuffer) ) { ; }
     while ( !postData(SENEC4, strBuffer) ) { ; }
     while ( !postData(SENEC5, strBuffer) ) { ; }
     while ( !postData(SENEC6, strBuffer) ) { ; }
     while ( !postData(SENEC7, strBuffer) ) { ; }
     while ( !postData(SENEC8, strBuffer) ) { ; }
     while ( !postData(SENEC9, strBuffer) ) { ; }
     while ( !postData(SENEC0, strBuffer) ) { ; }
   }

   /**********************************************************************************/
     
   // FERTILIZZANTE
   if ( currHour == 22 && currMin == 30  && statusFert == false) {
     apriGocciolatore();
     while ( !postData(FERTI1, "1") ) { ; }
     delay( 1300 ); // con 1300 millisecondi fa 10ml
     chiudiGocciolatore();
     while ( !postData(FERTI1, "0") ) { ; }
     
     statusFert = true;
   }
   if ( currHour == 22 && currMin == 35  && statusFert == true) { statusFert = false; }

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
   
   if (( 8 <= currHour && currHour <= 21 ) && (0 <= currMin && currMin <= 15)) {  // Sei in orario di irrigazione
      if ( !statusPompa ) {
        digitalWrite(pinPump1,HIGH); delay( 100 );
        digitalWrite(pinPump2,HIGH); delay( 100 );
        while ( !postData(POMPA1, "1") ) { ; }
        statusPompa = true;
      }
   } else {
      if ( statusPompa ) {
        digitalWrite(pinPump1,LOW); delay( 100 );
        digitalWrite(pinPump2,LOW); delay( 100 );
        while ( !postData(POMPA1, "0") ) { ; }
        statusPompa = false;
      }
   }
   
   /**********************************************************************************/
   delay( 60000 );
} // end loop
