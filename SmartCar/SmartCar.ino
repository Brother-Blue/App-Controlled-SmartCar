#include <Smartcar.h>

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

SimpleCar car(control);

const int maxObstacle = 20;
const int targetSpeed = 35;
const int speedStop = 0;
float duration, distance;
const int TRIGGER_PIN=5;
const int ECHO_PIN=18;
const unsigned int MAX_DISTANCE = 100;
SR04 front(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup(){
  //Serial monitoring
  Serial.begin(9600);

  //Initializing Triggerr output and echo input
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  }

void loop() {
  //Calls the drive function
 drive(targetSpeed);
}
 
void drive(int targetSpeed) {
  //Update the distance to object in front of car.
  int frontDistance = getFrontDistance();
  //Checks if distance to object is below our threshold 
  if(frontDistance > maxObstacle){
    car.setSpeed(targetSpeed);
  }else 
  car.setSpeed(speedStop);
}

long getFrontDistance() {
  //Set TRIGGER_PIN to low for 2 microseconds, just to make sure we start at low.
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  //Set TRIGGER_PIN to high for 10 microsecond to send out soundwave
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  //Messure the distance from obsticle by using pulseIn
  duration = pulseIn(ECHO_PIN, HIGH);
  //Convert distance to cm
  distance = (duration*.0343)/2;
  delay(100);
  //Return the distance messured
  return distance;
}
