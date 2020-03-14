#define input A0

int input_read;
void setup() {
  Serial.begin(38400);  //Utilizar una alta velocidad de comunicación serial.
  Serial.println("LABEL,Computer Time,READ");
}

void loop() {
  input_read = analogRead(input);
  Serial.print("DATA,TIME,");
  Serial.println(input_read);

}
