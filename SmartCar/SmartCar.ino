#include <Smartcar.h>
#include <BluetoothSerial.h>

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

BluetoothSerial bluetooth;

const int GYROSCOPE_OFFSET = 37;
GY50 gyro(GYROSCOPE_OFFSET); // The offset we have acquired via the getOffset method

const unsigned long PULSES_PER_METER = 600;
DirectionlessOdometer leftOdometer(smartcarlib::pins::v2::leftOdometerPin,[]() { leftOdometer.update(); }, PULSES_PER_METER);
DirectionlessOdometer rightOdometer(smartcarlib::pins::v2::rightOdometerPin, []() { rightOdometer.update(); }, PULSES_PER_METER);

const int fSpeed   = 70;  // 70% of the full speed forwards
const int bSpeed   = -70; // 70% of the full speed backwards
const int lDegrees = -75; // Degrees to turn left
const int rDegrees = 75;  // Degrees to turn right

SmartCar car(control, gyro, leftOdometer, rightOdometer);  // Initializing of the car

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
}

void handleInput(char input){ // Handle serial input if there is any
             
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
        
        case 'c': // Increases carspeed by 10%
            //car.setSpeed(car.getSpeed() + 10);
            break;

        case 'd': // Decreases carspeed by 10%
            // car.setSpeed(car.getSpeed() - 10);
            break;
            // TODO Implement SmartCar to access getSpeed-method. See new issue in Git
        default: 
            car.setSpeed(0);
            car.setAngle(0);
        }
    
}

void readBluetooth() {

    while (bluetooth.available()) {
        char input = bluetooth.read();
        handleInput(input);
    }
}

void loop() {
    readBluetooth();
    distance = front.getDistance();
    if(distance < minObstacle && distance > 0){
        car.setSpeed(0);
    }
 }