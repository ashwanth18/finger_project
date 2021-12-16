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
 
  // print the string when a newline arrives:
  if (stringComplete) {
     Serial.println("1");
    
    digitalWrite(13,HIGH);
    Serial.println(inputString);
    // clear the string:
    rotation = inputString;
    myservo.write(rotation.toInt()); // sets position to the scaled value
    inputString = "";
    
    stringComplete = false;
    for(int i = 0; i < count; i++){
      digitalWrite(13,HIGH);
    }
   
  }
}



void serialEvent() {
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = Serial.read();
    // add it to the inputString:
    inputString += inChar;
    count += 1;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      
    }
  }
  if (stringComplete){
  Serial.println("reiceved");
  }
}
