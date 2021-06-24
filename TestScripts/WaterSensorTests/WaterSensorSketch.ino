#define an0 A0


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(an0);
  // print out the value you read:
  Serial.println(sensorValue);
  delay(10);        // delay in between reads for stability
}
