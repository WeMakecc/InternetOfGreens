/*   Conduttimetro
Collegamenti hardware
sensore    rosso              blu                bianco
           P1 ------O  O------P2-----RRRRR-------P3        Lato sensore
   0       D2    elettrodi    A0    470 ohm      D3        Lato Arduino
   1       D8    elettrodi    A1    470 ohm      D9        Lato Arduino
*/ 
byte nsens = 2;
byte P1[] = {51,50};
byte P2[] = {A15,A14};
byte P3[] = {53,52};
// parametri di taratura in uS/cm
float A[] = {870.43,822.71};
float B[] = {-1.194,-1.190};
//  resistenza fissa tra P1 e P3
float R1 = 470.;  // ohm
//  inizializzo resistenza tra i due elettrodi
float RS[] = {0,0};
//  inizializzo conducibilità specifica
float CS[] = {0,0};
//  variabili di lavoro
byte isens = 0;
byte n = 50;

#define ECHO;

void setup () {  
    Serial.begin(9600);
    delay(100);
    //  setta come output le  porte digitali
    for(byte isens = 0; isens < nsens ; isens++){
      pinMode(P1[isens],OUTPUT);
      pinMode(P3[isens],OUTPUT);
    }
}

//////////////////////////////////////////////////////////
void loop () {
  
  Serial.print( "Loop sensore 1: " );
  Serial.println( readEC( 0 ) );
  Serial.print( "Loop sensore 2: " );
  Serial.println( readEC( 1 ) );
 
  delay( 100 );
}

//////////////////////////////////////////////////////////
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
    digitalWrite(P1[isens],HIGH);
    digitalWrite(P3[isens],LOW);
    runningAvg1 = runningAvg1 + Vconv*analogRead(P2[isens]);         
    digitalWrite(P1[isens],LOW);
    runningAvg2 = runningAvg2 + Vconv*analogRead(P2[isens]);         
    digitalWrite(P3[isens],HIGH);
    runningAvg3 = runningAvg3 + Vconv*analogRead(P2[isens]);         
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
      Serial.print(" sensore # ");
      Serial.print(isens);
      Serial.print("  Galvanica: ");
      Serial.print(VSG,2);
      Serial.print("  V1 = ");
      Serial.print(V1,0);
      Serial.print(" V2 = ");
      Serial.print(V2,0);
      Serial.print("  RS1 = ");
      Serial.print(RS1,0);
      Serial.print("  RS2 = ");
      Serial.print(RS2,0);
      Serial.print("  RS = ");
      Serial.print(RS[isens],3);
      Serial.print("  CS = ");
      Serial.print(CS[isens],0);
      Serial.println();
  #endif  
  
  return CS[isens];
}





