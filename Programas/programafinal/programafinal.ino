//Librerias
#include "HX711.h" //Modulo HX711 - Celda de carga

//Variables para la balanza
#define DOUT  A1
#define CLK  A0
float M = 872.23;//Pendiente de la curva característica
float B = -94929;//Intercepto en Y de la curva característica

//Variables para la mini bomba de agua
int Input1 = 9;    // Control pin 1 for motor 1

HX711 balanza;
                                                                                                          
//Variables para mini bomba de agua
int Densidad = 1;// en g/mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE DENSIDAD CORRESPONDIENTE!!!
int Volumen_deseado = 50;// en mL | MODIFIQUE PREVIAMENTE CON EL VALOR DE VOLUMEN DESEADO!!!

float pesoRecipiente=0;
float volumenLiquido=0;
float pesoFrut=0;
int ONOFF = 0;




//Funciones
//float MedidaBalanza();

//PID
float error;
float errorAnterior;
float Ts;
float accionP;
float accionI;
float accionD;
int accionDeControl;
float kp = 22;
float ki =250/350;
float kd = 0.009;
long tiempoActual;
long tiempoAnterior;



void setup() {

  Serial.begin(9600);
  balanza.begin(DOUT, CLK);
  // Inicializamos los pines
  pinMode( Input1, OUTPUT );
  analogWrite( Input1, 0 );
}


void loop() {

  medirRecipiente();
  //-------INPUT DATA-------
  Serial.println("Escriba la cantidad de miligramos de frutiño que va a ingresar y sirvala manualmente.");
  while(Serial.available() == false);
  float fruti = Serial.parseInt()/1000.0;
  Serial.end();
  Serial.begin(9600);
  //------------------------
  cargarFrut(fruti);
  //-------INPUT DATA-------
  Serial.println("Escriba la cantidad de líquido que desea ingresar al recipiente y pulse ENVIAR.");
  while(Serial.available() == false);
  float ref = Serial.parseInt()*1.0;
  Serial.end();
  Serial.begin(9600);
  //------------------------
  Serial.println("En 5 segundos se cargará el agua.");
  delay(5000);
  pesoRecipiente = MedidaBalanza();
  long tiempoInicial = millis();
  long tiempoActual;
  Serial.println("Input\tVolumen\tTiempo");
  do{
    tiempoActual = millis()-tiempoInicial;
    PID(ref);
    Serial.print(accionDeControl);
    Serial.print("\t");
    Serial.print(volumenLiquido);
    Serial.print("\t");
    Serial.println(tiempoActual);
  }
  while( (accionDeControl >= 20.0) && ( tiempoActual ) <= 20000 );
  analogWrite(Input1, 0);
  delay(3000);
  PID(ref);
  analogWrite(Input1, 0);
  Serial.print("Volumen final: ");
  Serial.print(volumenLiquido);
  Serial.println("mL");
  Serial.println("TERMINADO");

  //-------INPUT DATA-------
  Serial.println("Si desea hacer el proceso nuevamente, vacíe el recipiente y envíe cualquier dato.");
  while(Serial.available() == false);
  int descarte = Serial.parseInt();
  Serial.end();
  Serial.begin(9600);
  //------------------------
  delay(1000);
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
    peso = balanza.read_average(2); // Entrega el peso actualmente medido en gramos
    peso = (peso-B)/M;
    if (peso <= 0) peso = peso * -1;
    
    PL1 = peso;
    PL2 = PL1;
    PL3 = PL2;

    promPL = (PL1 + PL2 + PL3) / 3;
  }  
  //delay(25);
  return promPL;

}

void cargarFrut( float frutRequerido ){
  Serial.print("Ingrese ");
  Serial.print(frutRequerido);
  Serial.println("gr de frutiño.");
  do{
  pesoFrut = MedidaBalanza() - pesoRecipiente;
  Serial.print("Hay ");
  Serial.print(pesoFrut);
  Serial.println("gr.");
  delay(300);
  }
  while(pesoFrut <= frutRequerido);
  Serial.print("El peso final del frutiño  fue de ");
  Serial.print(pesoFrut);
  Serial.println("gr.");
}
void medirRecipiente(){
    Serial.println("DEJE SOLO EL RECIPIENTE EN LA BALANZA Y NO LO TOQUE");
    delay(2000);
    Serial.println("Calculando el peso del recipiente");
    Serial.print("3...");
    delay(1000);
    Serial.print("2...");
    delay(1000);
    Serial.println("1...");
    //analogWrite(Input1, 50);
    pesoRecipiente = MedidaBalanza();
    //analogWrite(Input1, 0);
    delay(1000);
    Serial.print("El peso es ");
    Serial.print(pesoRecipiente);
    Serial.println("gr.");
}
//referencia: Volumen del líquido al cual se desea llegar.
void PID( float referencia )
{
  volumenLiquido = (MedidaBalanza() - pesoRecipiente);
  error = referencia - volumenLiquido;
  tiempoActual = millis();
  Ts = tiempoActual-tiempoAnterior;
//  //
  accionP = kp*error;
  accionI += ki*error*Ts;
  accionD = kd*(error-errorAnterior)/Ts;
//  //
  tiempoAnterior = tiempoActual;
  errorAnterior = error;
//
//  //
  accionDeControl = accionP+accionI+accionD;
  if(accionDeControl > 255) accionDeControl = 255;
  if(accionDeControl < 0) accionDeControl = 0;
//
  analogWrite(Input1, accionDeControl);
}
