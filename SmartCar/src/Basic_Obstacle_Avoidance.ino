#include <Smartcar.h>

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

SimpleCar car(control);

const int TRIGGER_PIN=5;
const int ECHO_PIN=18;
const unsigned int MAX_DISTANCE = 100;
SR04 front(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup(){
  Serial.begin(9600);
  }

void loop() {
  while(front.getDistance()==0 || front.getDistance() > 15){
    car.setSpeed(50);
  }
  car.setSpeed(0);
  }
 
