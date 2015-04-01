#include <SPI.h>
#include <Ethernet.h>

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

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//char server_out[] = "iot.enter.it";    // name address for Google (using DNS)
IPAddress server_out(185,48,32,120); // iot.enter.it

IPAddress ip(192, 168, 2, 249);
EthernetClient client;

#define textBuffSize 500
int index;
char results[textBuffSize];
boolean startData = false; 

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
  // {"sensorId": "54ef29e7dd93d2dd01045dd7","value": "1.6"}
  postData(LIGHT1, "8.1");
  while(1);
}

boolean postData( const char* id, char* value) {
  if (client.connect(server_out, 80)) {
    String post = "{\"sensorId\":\""; post += id; post += "\",\"value\":\""; post += value; post += "\"}";

    Serial.println("connected");
    Serial.println(post.length());
    Serial.println(post);
    
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



