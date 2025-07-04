#include <Arduino.h>


#define leftSensor   A2
#define frontSensor  A1
#define rightSensor  A0


#define leftMotor1  4
#define leftMotor2  6
#define rightMotor1 5
#define rightMotor2 8

#define ledPin 13


#define WALL_THRESHOLD 200
#define STEP_DELAY 200


#define MAX_PATH_LENGTH 100
char path[MAX_PATH_LENGTH];
int pathLength = 0;


void setup() {
  pinMode(leftSensor, INPUT);
  pinMode(frontSensor, INPUT);
  pinMode(rightSensor, INPUT);
  
  pinMode(leftMotor1, OUTPUT);
  pinMode(leftMotor2, OUTPUT);
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, HIGH);
  delay(1000);
}


bool wallOnLeft()  { return analogRead(leftSensor)  > WALL_THRESHOLD; }
bool wallOnRight() { return analogRead(rightSensor) > WALL_THRESHOLD; }
bool wallAhead()   { return analogRead(frontSensor) > WALL_THRESHOLD; }


void goStraight() {
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(STEP_DELAY);
}

void turnLeft() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, HIGH);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(400);
}

void turnRight() {
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, HIGH);
  delay(400);
}

void turnAround() {
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, HIGH);
  delay(800);
}


void tremauxSolve() {
  if (!wallOnLeft()) {
    turnLeft();
    goStraight();
    path[pathLength++] = 'L';
  } else if (!wallAhead()) {
    goStraight();
    path[pathLength++] = 'S';
  } else if (!wallOnRight()) {
    turnRight();
    goStraight();
    path[pathLength++] = 'R';
  } else {
    turnAround();
    path[pathLength++] = 'B';  
  }

  if (pathLength >= MAX_PATH_LENGTH) {
    stopAndFlash();  
  }
}

void stopAndFlash() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  
  while (true) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }
}

void loop() {
  tremauxSolve();
}
