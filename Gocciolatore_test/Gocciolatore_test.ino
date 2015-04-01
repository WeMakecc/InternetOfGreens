/*  Programma di prova per l'elettrovalvola da giardino che
useremo per l'orto indoor.
Collegamenti:
fili del microswitch: uno a GND e l'altro a D11
Motore + al piedino 1 del Driver di Gianni e + 5V
Motore - al piedino 2 del Driver
Piedino 6 del Driver a GND
piedino 4 del Driver al D6
*/

#define pinMotore 3
#define pinValvola 2

void setup(){
  Serial.begin(9600);
  pinMode(pinMotore,OUTPUT);
  pinMode(pinValvola,INPUT);
  
  analogWrite(pinMotore,0);  // PWM di 55
  delay(1000);
  
}
void loop(){
  Serial.println( digitalRead(pinValvola) );
  apriGocciolatore();
  delay( 1300 );
  chiudiGocciolatore();
  delay( 10000 );
}

void apriGocciolatore() {
   Serial.println( "apri" );
   while ( digitalRead(pinValvola) == 1 ) {
     analogWrite(pinMotore,100);
   }
   analogWrite(pinMotore,0);
}

void chiudiGocciolatore() {
   Serial.println( "chiudi" );
   while ( digitalRead(pinValvola) == 0 ) {
     analogWrite(pinMotore,100);
   }
   delay( 100 );
   analogWrite(pinMotore,0);
}
