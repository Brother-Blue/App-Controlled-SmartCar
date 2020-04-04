#include <Smartcar.h>

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

SimpleCar car(control);

const int minObstacle = 20;
const int targetSpeed = 35;
float distance;

const int TRIGGER_PIN=5;
const int ECHO_PIN=18;
const unsigned int MAX_DISTANCE = 100;
SR04 front(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup(){
  Serial.begin(9600);
  car.setSpeed(targetSpeed);
}

void loop() {
 distance = front.getDistance();
 Serial.println(distance);
  
  
 if(distance < minObstacle && distance > 0){
  car.setSpeed(0);
 }
 
}
 
