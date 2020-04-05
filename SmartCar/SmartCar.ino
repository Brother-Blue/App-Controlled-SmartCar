#include <Smartcar.h>
#include <BluetoothSerial.h>

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

BluetoothSerial bluetooth;

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
  bluetooth.begin("Group 2 SmartCar");
  Serial.print("Ready to connect!");
  car.setSpeed(targetSpeed);
}

void loop() {
  inputHandler();
  
 distance = front.getDistance();
 Serial.println(distance);
  
  
 if(distance < minObstacle && distance > 0){
  car.setSpeed(0);
 }
 
}

void inputHandler() {
  if (bluetooth.available()) {
    //Handle user inputs
  }
}
 
