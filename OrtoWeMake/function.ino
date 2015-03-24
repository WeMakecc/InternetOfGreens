/**********************************************************************************/

void getApiSmartCitizen() {
  if (client.connect(server_in, 80)) {
    Serial.println("connected");
    client.println("GET /v0.0.1/26515ecad271367264ff71ee4c2e4fb3f24191bb/lastpost.json HTTP/1.1");
    client.println("Host: api.smartcitizen.me");
    client.println("Connection: close");
    client.println();
  }

  if (client.available())  {
     while (client.connected()) {
       char c = client.read();
       if( c == '{'  ){ if( startData==false){ startData=true; } }
       if(startData==true){ results[index] = c; index++; }
     } 
  }
  results[index]=0;
  
  if (!client.connected()) {
    Serial.println("disconnecting.");
    client.stop();
    startData=false; index = 0;
    StaticJsonBuffer<550> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(results);
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    //timestamp = root["devices"][0]["posts"]["timestamp"];
    temp  = root["devices"][0]["posts"]["temp"];
    hum   = root["devices"][0]["posts"]["hum"];
    co    = root["devices"][0]["posts"]["co"];
    no2   = root["devices"][0]["posts"]["no2"];
    noise = root["devices"][0]["posts"]["noise"];
    light = root["devices"][0]["posts"]["light"];
    batt  = root["devices"][0]["posts"]["bat"];
    
  }
}

/**********************************************************************************/

boolean postData( const char* id, char* value) {
  if (client.connect(server_out, 80)) {
    String post = "{\"sensorId\":\""; post += id; post += "\",\"value\":\""; post += value; post += "\"}";
    
    #ifdef DEBUG
        Serial.println("connected");
        Serial.println(post.length());
        Serial.println(post);
    #endif    

    client.println("POST /iog/samples HTTP/1.1");
    client.println("Host: iot.enter.it");
    client.println("Accept: */" "*");
    client.println("User-Agent: Arduino/1.0.0");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(post.length());
    client.println("Connection: close");
    client.println();
    client.print(post);
  }

  if (client.available())  {
    while (client.connected()) {
      char c = client.read();
      Serial.print( c );
      
      if( c == 'H'  ){ if(startData==false){ startData=true; } }
      if(startData==true){ results[index] = c; index++; }
      if (c == '\n') { startData=false;  } // Termina stringa caturata
    } 
  }
  results[index]=0;
  
  if (!client.connected()) {
    client.stop();
    startData=false; index = 0;
    if (results != "HTTP/1.1 200 OK") { return false; }
    else { return true; }
  }
}

/**********************************************************************************/

void getTime() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(1000);  
  if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);               

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;  
    // print Unix time:
    getHour( epoch );                               
    getMinute( epoch );                               
    getSecond( epoch );     

    Serial.println(currHour);
    Serial.println(currMin);
    Serial.println(currSec);
    
  }
}

/**********************************************************************************/

void getHour( unsigned long epoch ) {
    //sprintf(currHour, "%02d",((epoch  % 86400L) / 3600));
    currHour = (((epoch  % 86400L) / 3600) + 1);
}   

/**********************************************************************************/

void getMinute( unsigned long epoch ) {
    //sprintf(currMin, "%02d", ((epoch % 3600) / 60));
    currMin = ((epoch % 3600) / 60);
}

/**********************************************************************************/

void getSecond( unsigned long epoch ) {
    //sprintf(currSec, "%02d", (epoch % 60));
    currSec = (epoch % 60);
}

/**********************************************************************************/

// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 		   
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}

/**********************************************************************************/

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

/**********************************************************************************/

void getPh() {
  static float voltage;
  int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
  byte pHArrayIndex=0;  
  for (byte i = 0; i < ArrayLenth; i++ ) {
      pHArray[pHArrayIndex++]=analogRead(PhMeterPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
  }
  #ifdef DEBUG
    Serial.print("Voltage:");       Serial.print(voltage,2);
    Serial.print("    pH value: "); Serial.println(pHValue,2);
  #endif
}

/**********************************************************************************/

float readEC( byte isens ) {
  
  float RS1 = 0.;
  float RS2 = 0.;
  float VCC = 5000.;
  float Vconv = VCC/1024.;
  float V1 = 0.;
  float V2 = 0.;
  float VSG = 0.;
  
  float runningAvg1 = 0.;
  float runningAvg2 = 0.;
  float runningAvg3 = 0.;
  
  //  ciclo di misura su un sensore
  for(byte i = 0; i < n; i++) {
    digitalWrite(pinEC1[isens],HIGH);
    digitalWrite(pinEC3[isens],LOW);
    runningAvg1 = runningAvg1 + Vconv*analogRead(pinEC2[isens]);         
    digitalWrite(pinEC1[isens],LOW);
    runningAvg2 = runningAvg2 + Vconv*analogRead(pinEC2[isens]);         
    digitalWrite(pinEC3[isens],HIGH);
    runningAvg3 = runningAvg3 + Vconv*analogRead(pinEC2[isens]);         
  }
  VSG = runningAvg2/n;
  V1  = runningAvg1/n;      
  V2  = runningAvg3/n;      
  //  compute RH and VS
  if(V1 < 2.){
    RS[isens] = 60000.;
    CS[isens] = 0.;
  } else {
    //  correggo con la galvanica
    V1 = V1-VSG;
    V2 = V2-VSG;
    //  primo calcolo di RS
    RS1 = R1*(VCC-V1)/V1; 
    //  secondo calcolo di RS e media tra i due (RS in kohm)
    RS2 = R1*V2/(VCC-V2);
    RS[isens] = (RS1 + RS2)/2000.;
    //  calcolo CS secondo i parametri di taratura
    if( RS[isens] < 0.5) { CS[isens] = 1300; } else{ CS[isens] = A[isens]*pow(RS[isens],B[isens]); }          
  }
    
  #ifdef DEBUG
      Serial.print(" sensore # ");   Serial.print(isens);
      Serial.print("  Galvanica: "); Serial.print(VSG,2);
      Serial.print("  V1 = ");       Serial.print(V1,0);
      Serial.print(" V2 = ");        Serial.print(V2,0);
      Serial.print("  RS1 = ");      Serial.print(RS1,0);
      Serial.print("  RS2 = ");      Serial.print(RS2,0);
      Serial.print("  RS = ");       Serial.print(RS[isens],3);
      Serial.print("  CS = ");       Serial.println(CS[isens],0);
  #endif  
  
  return CS[isens];
}

/**********************************************************************************/

void apriGocciolatore() {
   Serial.println( "apri" );
   while ( digitalRead(pinValF1) == 1 ) {
     analogWrite(pinFert1,100);
   }
   analogWrite(pinFert1,0);
}

/**********************************************************************************/

void chiudiGocciolatore() {
   Serial.println( "chiudi" );
   while ( digitalRead(pinValF1) == 0 ) {
     analogWrite(pinFert1,100);
   }
   delay( 100 );
   analogWrite(pinFert1,0);
}

/**********************************************************************************/

/**********************************************************************************/



