#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include <Arduino_FreeRTOS.h>



SoftwareSerial mySoftwareSerial(11, 12); // RX, TX
DFRobotDFPlayerMini myDFPlayer;



void printDetail(uint8_t type, int value);
// Helper Vars.
int x=0;
char l;
int analogPin = A0;
 // Joystick Pin
int joystickPin = 13;
// ------------

// Adaptive Headlights
int headlightsSensorPin =A1;
int headlightsPin = 6;
// -------------------

//Gears Helper Vars
int val = 0;
int idx = 0;
int gears[4] = {1,0,0,0};
//             [P,R,N,D];
int gearsCounter=1;
// ----------------------

// Sensors' Pins
int leftSensorPin = 7;
int rightSensorPin = 8;
int leftMotorSpeedPin = 9;
int rightMotorSpeedPin = 10;
int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

// Task Signature
void checkForLaneCollisionTask (void *pvParameters);
void mp3PlayerTask (void *pvParameters);
void CITask (void *pvParameters);

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  speakerSettings();
  pinsSettings();
  wireSettings();

  xTaskCreate(checkForLaneCollisionTask, "checkForLaneCollision", 128, NULL, 3, NULL);
  xTaskCreate(CITask, "controlIndicators", 128, NULL, 2, NULL);
  xTaskCreate(mp3PlayerTask, "mp3Player", 128, NULL, 1, NULL);
}
void loop() {

}

void CITask(void *pvParameters){
  while(1){
    adaptiveHeadlights();
    gearsHandler();
  }
}

void mp3PlayerTask (void *pvParameters){
  while(1){
    //Play the first mp3
    if(x==1){
      myDFPlayer.next();  //Play next mp3
      x=0;
    }
    if(x==2){
      myDFPlayer.previous();  //Play previous mp3
      x=0;
    }
    if(x==3){
      myDFPlayer.pause();  //pause the mp3
      x=0;
    }if(x==4){
      myDFPlayer.start();  //start the mp3 from the pause
      x=0;
    }
  }
}

void gearsHandler(){
    if((analogRead(analogPin)<300||analogRead(analogPin)>700)){
      val = analogRead(analogPin);
      if (val > 700){
        if (idx < 3 ){
          idx++;
          resetGears();
          gears[idx]=1;
        }
      }
      if (val < 300){
        if (idx > 0 ){
          idx--;
          resetGears();
          gears[idx]=1;
        }
      }
      if (gears[0] == 1 &&gearsCounter!=1){
        gearsCounter=1;
        Wire.beginTransmission(5); // transmit to device #9
        Wire.write(1);              // sends x 
        Wire.endTransmission();    // stop transmitting
      }
      if (gears[1] == 1&&gearsCounter!=2){
        gearsCounter=2;
        Wire.beginTransmission(5); // transmit to device #9
        Wire.write(2);              // sends x 
        Wire.endTransmission();
      }
      if (gears[2] == 1&&gearsCounter!=3){
        gearsCounter=3;
        Wire.beginTransmission(5); // transmit to device #9
        Wire.write(3);              // sends x 
        Wire.endTransmission();
      }
      if (gears[3] == 1&&gearsCounter!=4){
        gearsCounter=4;
        Wire.beginTransmission(5); // transmit to device #9
        Wire.write(4);              // sends x 
        Wire.endTransmission();
      }
      
    }
    vTaskDelay(pdMS_TO_TICKS(150));
}

void adaptiveHeadlights(){
    if(analogRead(headlightsSensorPin)<100){
      analogWrite(headlightsPin, 0);
    }else if(analogRead(headlightsSensorPin)>100 && analogRead(headlightsSensorPin)<200){
      analogWrite(headlightsPin, 127);
    }else{ 
      analogWrite(headlightsPin, 255);
    }
}

void checkForLaneCollisionTask(void *pvParameters){
  while(1){
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, HIGH);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, HIGH);
      int leftSensorReadings = digitalRead(leftSensorPin);
      int rightSensorRedings = digitalRead(rightSensorPin);
      Serial.println("22");
      if (leftSensorReadings == 1 && rightSensorRedings != 1)
      {
        analogWrite(leftMotorSpeedPin,150); //ENA pin LEFT
        analogWrite(rightMotorSpeedPin,10);
      }
      else if(leftSensorReadings != 1 && rightSensorRedings == 1)
      {
        analogWrite(leftMotorSpeedPin,10);
        analogWrite(rightMotorSpeedPin,150);
      }
      else
      {
        analogWrite(leftMotorSpeedPin,60);
        analogWrite(rightMotorSpeedPin,60);
      }
      vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void resetGears(){
  gears[0] = 0;
  gears[1] = 0;
  gears[2] = 0;
  gears[3] = 0;
}

void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the   I2C
}

void speakerSettings(){
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  
  myDFPlayer.volume(20);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);
}

void pinsSettings(){
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);

  pinMode(headlightsPin, OUTPUT); 
  pinMode(headlightsSensorPin, INPUT);

  pinMode(joystickPin, OUTPUT);
  pinMode(analogPin,INPUT);
  
  digitalWrite(joystickPin, LOW);
}

void wireSettings(){
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9);   
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
}
