#include <Servo.h>
# include <stdlib.h>
#include<stdio.h>

Servo myservo; // create servo object to control a servo
// int potpin = 0; // analog pin used to connect the potentiometer

float val; // variable to read the value from the analog pin
String rotation;
String inputString = "";
bool stringComplete = false;
int count = 0;

void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  myservo.attach(9); // attaches the servo on pin 9 to the servo object
  inputString.reserve(200);

}



// serial EVENT

void loop() {
  while (Serial.available()) {
 char inChar = Serial.read();
 inputString += inChar;
 delay(100);
 if (inChar == '\n') {
      stringComplete = true;
    }
  }
if (stringComplete){
    Serial.println(inputString);
 myservo.write(inputString.toInt()); // sets position to the scaled value
 inputString = "";
}
}
