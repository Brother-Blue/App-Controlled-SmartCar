#include <Smartcar.h>
#include <BluetoothSerial.h>

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

BluetoothSerial bluetooth;

const int fSpeed   = 70;  // 70% of the full speed forwards
const int bSpeed   = -70; // 70% of the full speed backwards
const int lDegrees = -75; // Degrees to turn left
const int rDegrees = 75;  // Degrees to turn right

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

 handleInput();
  
 distance = front.getDistance();
 Serial.println(distance);
  
  
 if(distance < minObstacle && distance > 0){
  car.setSpeed(0);
  }
 }

 void handleInput(){ // Handle serial input if there is any
  
    if (Serial.available()){
        char input = Serial.read();
                                    
        switch (input){
        case 'l': // Turn Left
            car.setSpeed(fSpeed);
            car.setAngle(lDegrees);
            break;

        case 'r': // Turn Right
            car.setSpeed(fSpeed);
            car.setAngle(rDegrees);
            break;

        case 'f': // Foward
            car.setSpeed(fSpeed);
            car.setAngle(0);
            break;

        case 'b': // Reverse
            car.setSpeed(bSpeed);
            car.setAngle(0);
            break;

        default: 
            car.setSpeed(0);
            car.setAngle(0);
        }
    }
}

void inputHandler() {
  if (bluetooth.available()) {
    //Handle user inputs
  }
}
 
