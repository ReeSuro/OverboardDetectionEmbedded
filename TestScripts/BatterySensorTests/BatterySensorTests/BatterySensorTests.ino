void setup() {
  
  Serial.begin(9600);
}
int value = 0;
void loop() {
  value = analogRead(A0);
  Serial.println(value);
  delay(1000);
}
