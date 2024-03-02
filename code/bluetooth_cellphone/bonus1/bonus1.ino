#include  <SoftwareSerial.h>
SoftwareSerial BTSerial(10,11);
char val;


void setup() {
  Serial.begin(9600);
  Serial.println("BT is ready!");
  BTSerial.begin(38400);
}

void loop() {

  if (Serial.available()) {
    val = Serial.read();
    BTSerial.write(val);
  }
  if (BTSerial.available()) {
    val = BTSerial.read();
    Serial.print(val);
  }
}
