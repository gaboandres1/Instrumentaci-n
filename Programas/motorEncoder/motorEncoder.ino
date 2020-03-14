#include <Array.h>

/************************ Definición de pines y variables *************************/

// Puertos y Variables para Motor paso a paso
#define StepPin 4
#define ms1 5
#define ms2 6
#define ms3 7
//#define DirPin 3 // Este pin se pondra en GND ahora

unsigned int t = 3100; // Rango: 500 - 2700 que corresponde a 150 - 15 RPMs aprox.
unsigned int pasos = 1;
int contador = 0;
// Encoder

const int EncoderPin = 2;

int EncoderState;
int EncoderLastState;
const byte size = 20;
unsigned long rawArray[size] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
Array<unsigned long> array = Array<unsigned long>(rawArray,size);

float RPM;
int i_interrupt = 0;
unsigned long EncoderTime;
unsigned long dt;

/*********************************** Setup ***************************************/

void setup() {

Serial.begin(128000);
  pinMode( ms1 , OUTPUT);
  pinMode( ms2 , OUTPUT);
  pinMode( ms3 , OUTPUT);
//  pinMode( DirPin , OUTPUT);
  pinMode( StepPin , OUTPUT);
  ////// Motor paso a paso //////
  Serial.println("LABEL,Computer Time,Delay, RPM practico");
  ControlMotorPasoaPaso();

}

/************************************ Loop ***************************************/
void loop() {
  ////// Motor paso a paso //////
  t = t+100;
  long tiempoinicial = millis();
  long tiempo = 0;
  int vueltas = 0;
  while(tiempo <= 60000){
    for(int i=1;i<=200*pasos;i++){
        digitalWrite(StepPin, HIGH); 
        delayMicroseconds(t);           
        digitalWrite(StepPin, LOW);  
        delayMicroseconds(t);  
        medirRPM();
        //delay(1000);
      }
      //delay(1000);
      vueltas++;
      //Serial.println(vueltas);
      tiempo = millis() - tiempoinicial;
  }
  
  Serial.print("DATA,TIME, ");
  Serial.print(t);
  Serial.print(", ");
  Serial.println(vueltas);
  delay(500);

//  if(contador == 5){
//    while(true);
//  }
//  ////// Encoder //////

}

/*********************************** Funciones ***************************************/

void medirRPM()
{
  
  EncoderState = digitalRead(EncoderPin);
  
  if(EncoderState == LOW && EncoderLastState == 1){
    EncoderLastState = 0;
    EncoderTime = micros();
  }
  else if(EncoderState == HIGH && EncoderLastState == 0){
    dt = 2*(micros()-EncoderTime);
    array[ i_interrupt ] = dt;
    i_interrupt++;
    EncoderLastState = 1;
  }
//  else{
//    EncoderLastState = EncoderState;
//  }

  if(i_interrupt > size){
    i_interrupt = 0;
      RPM = array.getMax()/1000.0;
      RPM = 60.0*1e3/(20*RPM) ;
  }
}




//Función para Controlar el Motor Paso a Paso: Permite controlar el motor y los pasos de este
void ControlMotorPasoaPaso(void)
{
  
  switch(pasos){
    //Full Step
    case(1):
      digitalWrite(ms1, LOW);
      digitalWrite(ms2, LOW);
      digitalWrite(ms3, LOW);
    break;
    
    //Half Step
    case(2):
      digitalWrite(ms1, HIGH);
      digitalWrite(ms2, LOW);
      digitalWrite(ms3, LOW);
    break;
    
    //Quarter Step
    case(4):
      digitalWrite(ms1, LOW);
      digitalWrite(ms2, HIGH);
      digitalWrite(ms3, LOW);
    break;
    
    //Eighth Step
    case(8):
      digitalWrite(ms1, HIGH);
      digitalWrite(ms2, HIGH);
      digitalWrite(ms3, LOW);
    break;
    //Full Step
    default:
      pasos = 16;
      digitalWrite(ms1, HIGH);
      digitalWrite(ms2, HIGH);
      digitalWrite(ms3, HIGH);
    break;
  }

//  digitalWrite(DirPin, LOW);
}
