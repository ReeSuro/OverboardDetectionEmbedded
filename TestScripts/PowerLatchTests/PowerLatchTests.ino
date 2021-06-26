
#define latchPin D5
#define indiLED D2

void setup() {

  //1. Latch the microcontroller on 
  pinMode(latchPin, OUTPUT);
  digitalWrite(latchPin, HIGH);
  //2. Turn on indicator led
  pinMode(indiLED, OUTPUT);
  digitalWrite(indiLED, HIGH);
}

void loop() {
 delay(5000);
 //Turn off deviceby pulling latch pin low
 digitalWrite(indiLED, LOW);
 delay(1000);
 digitalWrite(indiLED, HIGH);
 delay(1000);
 digitalWrite(indiLED, LOW);
 digitalWrite(latchPin, LOW);
}
