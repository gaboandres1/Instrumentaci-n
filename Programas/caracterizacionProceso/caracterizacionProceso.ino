//Librerias
#include "HX711.h" //Modulo HX711 - Celda de carga

//Variables para la balanza
#define DOUT  A1
#define CLK  A0
float M = 872.23;//Pendiente de la curva característica
float B = -94929;//Intercepto en Y de la curva característica

//Variables para la mini bomba de agua
int Input1 = 9;    // Control pin 1 for motor 1

HX711 balanza(DOUT, CLK);
                                                                                                          
//Variables para mini bomba de agua
int Densidad = 1;// en g/mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE DENSIDAD CORRESPONDIENTE!!!
int Volumen_deseado = 50;// en mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE VOLUMEN DESEADO!!!

float pesoRecipiente;



//Funciones
//float MedidaBalanza();

void setup() {

  Serial.begin(9600);

  // Inicializamos los pines
  pinMode( Input1, OUTPUT);
  
  Serial.println("DEJE SOLO EL RECIPIENTE EN LA BALANZA Y NO LO TOQUE");
  delay(2000);
  Serial.println("Calculando el peso del recipiente");
  Serial.print("3...");
  delay(1000);
  pesoRecipiente = MedidaBalanza();
  Serial.print("2...");
  delay(1000);
  pesoRecipiente += MedidaBalanza();
  Serial.println("1...");
  delay(1000);
  pesoRecipiente += MedidaBalanza();
  pesoRecipiente *= 1/3.0;
  Serial.print("El peso es ");
  Serial.print(pesoRecipiente);
  Serial.println("gr.");
}


void loop() {


  Serial.println("Input\tMasa\tTiempo");
  float volumenLiquido;
  analogWrite(Input1, 255);
  delay(3000);
  analogWrite(Input1, 0);

  long tiempo = 0;
  int input = 0;
  long tiempoInicial = millis();

  input = 30;
  analogWrite(Input1, input);
  while( tiempo<=3000 ){
    tiempo = millis()-tiempoInicial;
    volumenLiquido = MedidaBalanza() - pesoRecipiente;
    Serial.print(input);
    Serial.print("\t");
    Serial.print(volumenLiquido);
    Serial.print("\t");
    Serial.println(tiempo);
  }

  input = 255;
  analogWrite(Input1, input);
  while( tiempo<=6000 ){
    tiempo = millis()-tiempoInicial;
    volumenLiquido = MedidaBalanza() - pesoRecipiente;
    Serial.print(input);
    Serial.print("\t");
    Serial.print(volumenLiquido);
    Serial.print("\t");
    Serial.println(tiempo);
  }
  
  input = 150;
  analogWrite(Input1, input);
  while( tiempo<=10000 ){
    tiempo = millis()-tiempoInicial;
    volumenLiquido = MedidaBalanza() - pesoRecipiente;
    Serial.print(input);
    Serial.print("\t");
    Serial.print(volumenLiquido);
    Serial.print("\t");
    Serial.println(tiempo);
  }
  
  input = 80;
  analogWrite(Input1, input);
  while( tiempo<=16000 ){
    tiempo = millis()-tiempoInicial;
    volumenLiquido = MedidaBalanza() - pesoRecipiente;
    Serial.print(input);
    Serial.print("\t");
    Serial.print(volumenLiquido);
    Serial.print("\t");
    Serial.println(tiempo);
  }

  Serial.println("FINALIZADO");
  while(true);

}

// FUNCIONES


//Función de Medición de Balanza: Permite obtener la medida actual en peso (g) de la balanza
float MedidaBalanza()
{
  float peso; // variable para el peso actualmente medido en gramos
  float PL1 = 0;
  float PL2 = 0;
  float PL3 = 0;
  float promPL = 0;

  for (int i = 1; i >= 0; i--)
  {
    peso = balanza.read_average(3); // Entrega el peso actualmente medido en gramos
    peso=(peso-B)/M;
    if (peso <= 0) peso = peso * -1;
    
    PL1 = peso;
    PL2 = PL1;
    PL3 = PL2;

    promPL = (PL1 + PL2 + PL3) / 3;
  }  
  //delay(25);
  return promPL;

}
