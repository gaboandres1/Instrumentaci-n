/*
   Instrumentación Electrónica _ 2020-01
   Laboratorio N°1
   Profesor: Johann F. Osma
   Asistente: Juliana Noguera Contreras
*/

//Librerias
#include "HX711.h" //Modulo HX711 - Celda de carga

//Variables para la balanza
#define DOUT  A1
#define CLK  A0

//Variables para la mini bomba de agua
int Input1 = 9;    // Control pin 1 for motor 1
int Input2 = 10;     // Control pin 2 for motor 1

//Variables para calibracion de balanza
int Peso_conocido = 156;// en gramos | MODIFIQUE PREVIAMENTE CON EL VALOR DE PESO CONOCIDO!!!

HX711 balanza(DOUT, CLK);
                                                                                                          
//Variables para mini bomba de agua
int Densidad = 1;// en g/mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE DENSIDAD CORRESPONDIENTE!!!
int Volumen_deseado = 50;// en mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE VOLUMEN DESEADO!!!

int OnOff = 1; // Switch On/Off para la bomba (1 = On | 0 = Off)
int Adelante; // Variable PWM 1
int Atras; // Variable PWM 2
int Detener = map(0, 0, 1023, 0, 255);
int Var = 1022;
float pesoRecipiente = 0;

//PID
float error;
float accionP;
float accionI;
int accionDeControl;
float kp=1;
float ki=1;
long tiempoActual;
long tiempoAnterior;

void setup() {

  // Balanza
//  CalibracionBalanza();
//  Serial.println(" ");
//  Serial.println("¡¡¡LISTO PARA PESAR!!!");
//  Serial.println(" ");
//  Serial.println(" ");
Serial.begin(9600);

  // Inicializamos los pines
  pinMode( Input1, OUTPUT);
  pinMode( Input2, OUTPUT);
}


void loop() {
  if (OnOff == 1) {
    for (int i = 3; i >= 0; i--)
   {
       MedidaBalanza(); //Medición actual de la Balanza
    }
    Serial.println(" ");
    Serial.println("¡¡¡LISTO PARA DOSIFICAR!!!");
    Serial.println(" ");
    Serial.println("Coloque el recipiente y no lo retire hasta finalizar");
    Serial.println(" ");
    Serial.println(" ");
    delay(100);

   pesoRecipiente = MedidaBalanza();
   float tiempoActual = millis();
   DosificarB(pesoRecipiente); //Dosificación de volumen según Balanza
   tiempoActual = millis() - tiempoActual;
   float pesoLiquido = MedidaBalanza() - pesoRecipiente;
   Serial.print("El peso del liquido es de ");
   Serial.print(pesoLiquido);
   Serial.print("g.\t");
   Serial.print(tiempoActual);
   Serial.println("ms.");
   delay(5000);
   Serial.print("Peso del líquido: ");
   Serial.print( MedidaBalanza()-pesoRecipiente );
   Serial.println("g");
  }

  



}

// FUNCIONES


//Función de Medición de Balanza: Permite obtener la medida actual en peso (g) de la balanza
float MedidaBalanza(void)
{
  float peso; // variable para el peso actualmente medido en gramos
  float PL1 = 0;
  float PL2 = 0;
  float PL3 = 0;
  float promPL = 0;

  for (int i = 1; i >= 0; i--)
  {
    peso = balanza.read_average(3); // Entrega el peso actualmente medido en gramos
    peso=(peso+94929)/872.23;
    if (peso <= 0) peso = peso * -1;
    
    PL1 = peso;
    PL2 = PL1;
    PL3 = PL2;

    promPL = (PL1 + PL2 + PL3) / 3;
  }  

  //delay(25);

  return promPL;

}

//Función de DosificarB: Permite entregar un volumen deseado(mL) según la medida actual de peso (g) de la balanza
void PID( float referencia )
{

  error = referencia - (MedidaBalanza-pesoRecipiente);
  tiempoActual = millis();

  //
  accionP = kp*error;
  accionI += ki*error/(tiempoActual-tiempoAnterior);
  //
  tiempoAnterior = tiempoActual;

  //
  accionDeControl = accionP+accionI;
  if(accionDeControl > 255) accionDeControl = 255;
  else if(accionDeControl <0) accionDeControl = 0;

  analogWrite(Input1, accionDeControl);
}
