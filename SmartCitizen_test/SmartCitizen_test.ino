#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//char server_in[] = "api.smartcitizen.me";    // name address for Google (using DNS)
IPAddress server_in(104,28,14,99); // api.smartcitizen.me

#define textBuffSize 500
int index;
char results[textBuffSize];
boolean startData = false; 

const char* timestamp;
double temp,hum,co,no2,noise,light,batt;
  
IPAddress ip(192, 168, 2, 249);
EthernetClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
}

void loop() {
   getApiSmartCitizen();
   if ( results[0] == '{' ) {
     //Serial.println( "===== Results =====" );
     //Serial.println( results );
     /* DEBUG */
    Serial.print("timestamp "); Serial.println(timestamp);
    Serial.print("temp "); Serial.println(temp);
    Serial.print("hum "); Serial.println(hum);
    Serial.print("co "); Serial.println(co);
    Serial.print("no2 "); Serial.println(no2);
    Serial.print("noise "); Serial.println(noise);
    Serial.print("light "); Serial.println(light);
    Serial.print("batt "); Serial.println(batt);
    /* */
     while( 1 );
   }
}

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
    timestamp = root["devices"][0]["posts"]["timestamp"];
    temp  = root["devices"][0]["posts"]["temp"];
    hum   = root["devices"][0]["posts"]["hum"];
    co    = root["devices"][0]["posts"]["co"];
    no2   = root["devices"][0]["posts"]["no2"];
    noise = root["devices"][0]["posts"]["noise"];
    light = root["devices"][0]["posts"]["light"];
    batt  = root["devices"][0]["posts"]["bat"];
    
  }

}


