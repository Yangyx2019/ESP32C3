#include <Arduino.h>
 
void setup() {
  // put your setup code here, to run once:
    pinMode(8, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
    printf("111111\n");
    delay(1000);                       // wait for a second
    digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);
}