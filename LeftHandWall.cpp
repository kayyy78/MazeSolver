
#include <Arduino.h>

#define leftCenterSensor   A3
#define leftNearSensor     A4
#define leftFarSensor      A5
#define rightCenterSensor  A2
#define rightNearSensor    A1
#define rightFarSensor     A0

int leftCenterReading;
int leftNearReading;
int leftFarReading;
int rightCenterReading;
int rightNearReading;
int rightFarReading;

int leftNudge;
int replaystage;
int rightNudge;

#define leapTime 200

#define leftMotor1  4
#define leftMotor2  6
#define rightMotor1 5
#define rightMotor2 8
#define led 13

char path[30] = {};
int pathLength;
int readLength;

void readSensors();
void leftHandWall();
void done();
void turnLeft();
void turnRight();
void straight();
void turnAround();
void shortPath();
void printPath();
void replay();
void endMotion();

void setup() {
  pinMode(leftCenterSensor, INPUT);
  pinMode(leftNearSensor, INPUT);
  pinMode(leftFarSensor, INPUT);
  pinMode(rightCenterSensor, INPUT);
  pinMode(rightNearSensor, INPUT);
  pinMode(rightFarSensor, INPUT);

  pinMode(leftMotor1, OUTPUT);
  pinMode(leftMotor2, OUTPUT);
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(led, HIGH);
  delay(1000);
}

void loop() {
  readSensors();
  if (leftFarReading < 200 && rightFarReading < 200 && (leftCenterReading > 200 || rightCenterReading > 200)) {
    straight();
  } else {
    leftHandWall();
  }
}

void readSensors() {
  leftCenterReading  = analogRead(leftCenterSensor);
  leftNearReading    = analogRead(leftNearSensor);
  leftFarReading     = analogRead(leftFarSensor);
  rightCenterReading = analogRead(rightCenterSensor);
  rightNearReading   = analogRead(rightNearSensor);
  rightFarReading    = analogRead(rightFarSensor);
}

void leftHandWall() {
  if (leftFarReading > 200 && rightFarReading > 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(leapTime);
    readSensors();

    if (leftFarReading > 200 || rightFarReading > 200) {
      done();
    } else if (leftFarReading < 200 && rightFarReading < 200) {
      turnLeft();
    }
  }

  if (leftFarReading > 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(leapTime);
    readSensors();

    if (leftFarReading < 200 && rightFarReading < 200) {
      turnLeft();
    } else {
      done();
    }
  }

  if (rightFarReading > 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(10);
    readSensors();

    if (leftFarReading > 200) {
      delay(leapTime - 30);
      readSensors();

      if (rightFarReading > 200 && leftFarReading > 200) {
        done();
      } else {
        turnLeft();
        return;
      }
    }

    delay(leapTime - 30);
    readSensors();

    if (leftFarReading < 200 && leftCenterReading < 200 && rightCenterReading < 200 && rightFarReading < 200) {
      turnRight();
      return;
    }

    path[pathLength] = 'S';
    pathLength++;
    if (path[pathLength - 2] == 'B') {
      shortPath();
    }
    straight();
  }

  readSensors();
  if (leftFarReading < 200 && leftCenterReading < 200 && rightCenterReading < 200 &&
      rightFarReading < 200 && leftNearReading < 200 && rightNearReading < 200) {
    turnAround();
  }
}

void done() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  replaystage = 1;
  path[pathLength] = 'D';
  pathLength++;
  while (analogRead(leftFarSensor) > 200) {
    digitalWrite(led, LOW);
    delay(150);
    digitalWrite(led, HIGH);
    delay(150);
  }
  delay(500);
  replay();
}

void turnLeft() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(1075);
  if (replaystage == 0) {
    path[pathLength] = 'L';
    pathLength++;
    if (path[pathLength - 2] == 'B') {
      shortPath();
    }
  }
}

void turnRight() {
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  delay(1075);
  if (replaystage == 0) {
    path[pathLength] = 'R';
    pathLength++;
    if (path[pathLength - 2] == 'B') {
      shortPath();
    }
  }
}

void straight() {
  if (analogRead(leftCenterSensor) < 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(1);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(5);
    return;
  }
  if (analogRead(rightCenterSensor) < 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(1);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    delay(5);
    return;
  }
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(4);
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  delay(1);
}

void turnAround() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, HIGH);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(900);
  path[pathLength] = 'B';
  pathLength++;
  straight();
}

void shortPath() {
  int shortDone = 0;
  if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'R') {
    pathLength -= 3;
    path[pathLength] = 'B';
    shortDone = 1;
  } else if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'S') {
    pathLength -= 3;
    path[pathLength] = 'R';
    shortDone = 1;
  } else if (path[pathLength - 3] == 'R' && path[pathLength - 1] == 'L') {
    pathLength -= 3;
    path[pathLength] = 'B';
    shortDone = 1;
  } else if (path[pathLength - 3] == 'S' && path[pathLength - 1] == 'L') {
    pathLength -= 3;
    path[pathLength] = 'R';
    shortDone = 1;
  } else if (path[pathLength - 3] == 'S' && path[pathLength - 1] == 'S') {
    pathLength -= 3;
    path[pathLength] = 'B';
    shortDone = 1;
  } else if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'L') {
    pathLength -= 3;
    path[pathLength] = 'S';
    shortDone = 1;
  }
  path[pathLength + 1] = 'D';
  path[pathLength + 2] = 'D';
  pathLength++;
}

void printPath() {
  for (int x = 0; x <= pathLength; x++) {
    Serial.println(path[x]);
  }
}

void replay() {
  readSensors();
  if (leftFarReading < 200 && rightFarReading < 200) {
    straight();
  } else {
    char move = path[readLength];
    if (move == 'D') {
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(100);
      digitalWrite(leftMotor1, LOW);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, LOW);
      digitalWrite(rightMotor2, LOW);
      endMotion();
    } else if (move == 'L') {
      delay(leapTime);
      turnLeft();
    } else if (move == 'R') {
      delay(leapTime);
      turnRight();
    } else if (move == 'S') {
      delay(leapTime);
      straight();
    }
    readLength++;
  }
  replay();
}

void endMotion() {
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(500);
  endMotion();
}
