#include <Servo.h>

const int M1_AIN1 = 22;  
const int M1_AIN2 = 23;  
const int M1_PWMA = 12;  // PWM

const int M2_BIN1 = 24;  
const int M2_BIN2 = 25;  
const int M2_PWMB = 11;  // PWM

// board2
const int M3_AIN1 = 26;
const int M3_AIN2 = 27;
const int M3_PWMA = 10;  // PWM

const int M4_BIN1 = 28;
const int M4_BIN2 = 29;
const int M4_PWMB = 9;  // PWM

const int defaultPWM1 = 100; 
const int defaultPWM2 = 120; // 110
const int defaultPWM3 = 110;
const int defaultPWM4 = 115; // 100

int motorPWM1 = defaultPWM1; 
int motorPWM2 = defaultPWM2;
int motorPWM3 = defaultPWM3;
int motorPWM4 = defaultPWM4;

Servo armServo;
Servo penServo;
const int armServoPin = 8;
const int penServoPin = 3;

const int echo_r = 50;
const int trig_r = 51;
const int echo_f = 52;
const int trig_f = 53;


unsigned long duration = 0;
unsigned long distance_f;
unsigned long distance_r;
unsigned long lastDistance[3] = {999,999,999};

const int ABUpper_r = 200;
const int ABLower_r = 160;
const int waypointB_f = 190;
const int doorClear = 140;
const int WBClear = 105;

const int delay180 = 3450;
const int delay90 = 1800;

unsigned long lastMillis = 0;
unsigned long lastMillis1 = 0;
int procStep = 0;
int step = 0;

void setup() {

  // Motor1 (front-left)
  pinMode(M1_AIN1, OUTPUT);
  pinMode(M1_AIN2, OUTPUT);
  pinMode(M1_PWMA, OUTPUT);

  // Motor2 (front-right)
  pinMode(M2_BIN1, OUTPUT);
  pinMode(M2_BIN2, OUTPUT);
  pinMode(M2_PWMB, OUTPUT);

  // Motor3 (rear-left)
  pinMode(M3_AIN1, OUTPUT);
  pinMode(M3_AIN2, OUTPUT);
  pinMode(M3_PWMA, OUTPUT);

  // Motor4 (rear-right)
  pinMode(M4_BIN1, OUTPUT);
  pinMode(M4_BIN2, OUTPUT);
  pinMode(M4_PWMB, OUTPUT);

  armServo.attach(armServoPin);
  penServo.attach(penServoPin);

  pinMode(trig_f,OUTPUT);
  pinMode(echo_f,INPUT);
  pinMode(trig_r,OUTPUT);
  pinMode(echo_r,INPUT);

  armServo.write(145);
  penServo.write(180);

  stopAll();
}

void loop() {
  distance_r = USDistance(0);
  distance_f = USDistance(1);
  switch (procStep){ 
    case 0: 
      if (millis()-lastMillis > 5000){
        rotateCW();
        procStep = 1;
        lastMillis = millis();
      }
      break;

    case 1:
      
      if (millis()-lastMillis > delay180){
        forwardAll();
        procStep = 2;
        lastMillis = millis();
      }
      
      break;

    case 2: // AB
      
      if (distance_r > ABUpper_r){
        diagonalFR();    
      }else if (distance_r < ABLower_r){
        diagonalFL();  
      }else{
        forwardAll();
      } 
      
      if (millis() - lastMillis1 > 50){
        switch (step){
          case 0:
            lastDistance[0] = distance_f;
            step = 1;
            break;
          case 1:
            lastDistance[1] = distance_f;
            step = 2;
            break;
          case 2:
            lastDistance[2] = distance_f;
            step = 0;
            break;
        }    
        lastMillis1 = millis();
      }

      if (lastDistance[0] < waypointB_f && lastDistance[1] < waypointB_f && lastDistance[2] < waypointB_f){
        rotateCCW();
        procStep = 3;
        lastMillis = millis();
        lastDistance[0] = 999;
        lastDistance[1] = 999;
        lastDistance[2] = 999;
      }
      break;

    case 3:
      
      if (millis() - lastMillis > delay90){
        forwardAll();
        procStep = 4;
        lastMillis = millis();
      }
      break;

    case 4:
   
      if (distance_r > ABUpper_r){
        diagonalFR();
      }else if (distance_r < ABLower_r && distance_r > doorClear){
        diagonalFL();
      }else{
        forwardAll();
      } 
      
      if (millis() - lastMillis1 > 50){
        switch (step){
          case 0:
            lastDistance[0] = distance_f;
            step = 1;
            break;
          case 1:
            lastDistance[1] = distance_f;
            step = 2;
            break;
          case 2:
            lastDistance[2] = distance_f;
            step = 0;
            break;
        }    
        lastMillis1 = millis();
      }

      if (lastDistance[0] < WBClear && lastDistance[1] < WBClear && lastDistance[2] < WBClear){
        stopAll();
        procStep = 5;
        lastMillis = millis();
        lastDistance[0] = 999;
        lastDistance[1] = 999;
        lastDistance[2] = 999;
      }

      break;

    case 5:
      delay(1000);
      armServo.write(0);
      delay(1000);
      armServo.write(140);
      delay(500);
      armServo.write(145);
      backwardAll();
      delay(500);
      

      rotateCCW();
      procStep = 6;
      lastMillis = millis();

      break;
    
    case 6:
      if (millis() - lastMillis > delay90){
        forwardAll();
        procStep = 7;
        lastMillis = millis();
      }
      break;
    
    case 7:
      if (distance_r > ABUpper_r){
        diagonalFR();
      }else if (distance_r < ABLower_r){
        diagonalFL();     
      }else{
        forwardAll();
      } 
      if (millis() - lastMillis1 > 50){
        switch (step){
          case 0:
            lastDistance[0] = distance_f;
            step = 1;
            break;
          case 1:
            lastDistance[1] = distance_f;
            step = 2;
            break;
          case 2:
            lastDistance[2] = distance_f;
            step = 0;
            break;
        }    
        lastMillis1 = millis();
      }

      if (lastDistance[0] < waypointB_f && lastDistance[1] < waypointB_f && lastDistance[2] < waypointB_f){
        rotateCCW();
        procStep = 8;
        lastMillis = millis();
        lastDistance[0] = 999;
        lastDistance[1] = 999;
        lastDistance[2] = 999;
      }
      break;
    
    case 8:
      if (millis() - lastMillis > delay90){
        forwardAll();
        procStep = 9;
        lastMillis = millis();
      }
      break;
    
    case 9:
      if (distance_r > ABUpper_r){
        diagonalFR();
      }else if (distance_r < ABLower_r && distance_r > doorClear){
        diagonalFL();
      }else{
        forwardAll();
      } 
      if (millis() - lastMillis1 > 50){
        switch (step){
          case 0:
            lastDistance[0] = distance_f;
            step = 1;
            break;
          case 1:
            lastDistance[1] = distance_f;
            step = 2;
            break;
          case 2:
            lastDistance[2] = distance_f;
            step = 0;
            break;
        }    
        lastMillis1 = millis();
      }

      if (lastDistance[0] < waypointB_f && lastDistance[1] < waypointB_f && lastDistance[2] < waypointB_f){
        stopAll();
        procStep = 10;
        lastMillis = millis();
        lastDistance[0] = 999;
        lastDistance[1] = 999;
        lastDistance[2] = 999;
      }
      break;
  
    case 10:
      penServo.write(60);
      delay(1000);
      for (int i = 0; i <= 10 ; i++){
      penServo.write(180);
      delay(250);
      penServo.write(90);
      delay(250);
      }
      procStep = 11;
      break;
    
  }
}

void setMotor(int in1, int in2, int pwmPin, int speedVal, int dir) {

  if (speedVal < 0)   speedVal = 0;
  if (speedVal > 255) speedVal = 255;

  switch (dir) {
    case 1: // forward 
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      analogWrite(pwmPin, speedVal);
      break;
    case -1: // reverse 
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      analogWrite(pwmPin, speedVal);
      break;
    case 0: // stop
      digitalWrite(in1, HIGH);
      digitalWrite(in2, HIGH);
      break;
  }
}


void forwardAll() {
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1,  1);  
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2,  -1);
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3,  1);  
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4,  -1);  
}


void backwardAll() {
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1, -1);
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2, 1);
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3, -1);
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4, 1);
}


void strafeLeft() {
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1, -1); 
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2, -1); 
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3, 1); 
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4, 1); 
}


void strafeRight() {
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1, 1); 
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2, 1); 
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3, -1); 
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4, -1); 
}


void rotateCW() {
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1, 1); 
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2,  1); 
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3, 1); 
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4,  1); 
}


void rotateCCW() {
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1,  -1); 
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2, -1); 
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3,  -1); 
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4, -1); 
}

void diagonalFL(){
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, 0, 1);
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2, -1);
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3, 1);
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, 0, -1);
}

void diagonalFR(){
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1, 1);
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, 0, -1);
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, 0, 1);
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4, -1);
}

void stopAll() {
  setMotor(M1_AIN1, M1_AIN2, M1_PWMA, motorPWM1, 0);
  setMotor(M2_BIN1, M2_BIN2, M2_PWMB, motorPWM2, 0);
  setMotor(M3_AIN1, M3_AIN2, M3_PWMA, motorPWM3, 0);
  setMotor(M4_BIN1, M4_BIN2, M4_PWMB, motorPWM4, 0);
}

unsigned long USDistance(bool side){
  float distance;
  if (side == 1){
    digitalWrite(trig_f, 0);
    delayMicroseconds(2);
    digitalWrite(trig_f, 1);
    delayMicroseconds(10);
    digitalWrite(trig_f, 0);

    duration = pulseIn(echo_f,1);
    distance = duration*0.1715;
    return distance;
  }else{
    digitalWrite(trig_r, 0);
    delayMicroseconds(2);
    digitalWrite(trig_r, 1);
    delayMicroseconds(10);
    digitalWrite(trig_r, 0);

    duration = pulseIn(echo_r,1);
    distance = duration*0.1715;
    return distance;
  }
}
