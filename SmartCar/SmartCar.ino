#include <Smartcar.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <VL53L0X.h>

BluetoothSerial bluetooth;

const int speed = 70;       // 70% of the full speed
const int turnAngle = 75;   // Degrees to turn
const int minObstacle = 20; // Minimum distance ahead to obstacle
const int GYROSCOPE_OFFSET = 37;
unsigned int error = 0; //FIXME: Need to calculate error for LIDAR sensor readings.
const unsigned int MAX_DISTANCE = 100; // Max distance to measure with ultrasonic

float frontDistance;
float backDistance;

boolean atObstacle = false;

//FIXME: Make sure ultrasonic pins are correct for side or behind :)
const int TRIGGER_PIN = 5; // Trigger signal for ultrasonic
const int ECHO_PIN = 18; // Reads signal for ultrasonic

//FIXME: Make sure this is the correct pin on the vehicle :)
const int SIDE_FRONT_PIN = A0; // LIDAR Sensor pin

const unsigned long PULSES_PER_METER = 600; // Amount of odometer pulses per 1 meter

SR04 back(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Ultrasonic sensor
VL53L0X frontSensor;

GY50 gyro(GYROSCOPE_OFFSET); // Gyroscope sensor const int rDegrees = 75;  // Degrees to turn right

DirectionlessOdometer leftOdometer(
    smartcarlib::pins::v2::leftOdometerPin, []() { leftOdometer.update(); }, PULSES_PER_METER); // Odometer sensor
DirectionlessOdometer rightOdometer(
    smartcarlib::pins::v2::rightOdometerPin, []() { rightOdometer.update(); }, PULSES_PER_METER);

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins); // Car motors
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

SmartCar car(control, gyro, leftOdometer, rightOdometer); // Initializing of the car

void setup()
{
    Serial.begin(9600);
    frontSensor.init();
    Wire.begin();
    bluetooth.begin("Group 2 SmartCar");
    Serial.print("Ready to connect!");
    frontSensor.setTimeout(500);
    frontSensor.startContinuous(100);
}

void turnRight(int degrees, int turnSpeed = speed)
{
    car.setSpeed(turnSpeed);
    car.setAngle(degrees);
}

void turnLeft(int degrees, int turnSpeed = speed)
{
    car.setSpeed(turnSpeed);
    car.setAngle(degrees);
}

void driveForward(int driveSpeed = speed)
{
    if (car.getSpeed() < 0)
    {                                // Fixes motor overloading
        while (car.getSpeed() < -20) // Used -20 as a threshold
        {
            car.setSpeed(car.getSpeed() + 10);
        }
    }
    car.setSpeed(driveSpeed);
    car.setAngle(0);
}

void driveBackward(int driveSpeed = speed)
{
    if (car.getSpeed() > 0)
    {                               // Fixes motor overloading
        while (car.getSpeed() > 20) // Used 20 as a threshold
        {
            car.setSpeed(car.getSpeed() - 10);
        }
    }
    car.setAngle(0);
    car.setSpeed(driveSpeed);
}

void brake() {
    car.setSpeed(0);
    car.setAngle(0);
}

boolean tryTurning()
{
    while (atObstacle)
    {
        // Try turning right
        for (int i = car.getHeading(); i < turnAngle; i += 5)
        {
            // Increment steering to the right and check if space is free
            turnRight(i, 20);
            delay(100);
            if (frontDistance > 65) // 65 was used as 78 is the max range for the LIDAR sensor
            {
                atObstacle = false;
                break;
            }
        }
        // Try turning left
        for (int i = car.getHeading(); i > turnAngle; i -= 5)
        {
            // Increment steering to the right and check if space is free
            turnLeft(i, 20);
            delay(100);
            if (frontDistance > 65) // 65 was used as 78 is the max range for the LIDAR sensor
            {
                atObstacle = false;
                break;
            }
        }
        car.setAngle(0);
        if (atObstacle)
            return true;
    }
    return atObstacle;
}

void driveWithAvoidance()
{
    if (!atObstacle) { driveForward(speed); }
    frontDistance = (frontSensor.readRangeContinuousMillimeters()*10) - error; //Multiplied by 10 to convert to cm
    if (backDistance > 0 && backDistance <= minObstacle)
    {
        brake();
    }
    if (frontDistance > 0 && frontDistance <= minObstacle)
    {
        atObstacle = true;
        brake();
        while (atObstacle && backDistance >= minObstacle)
        {
            driveBackward(-10);
            backDistance = back.getDistance();
            if (backDistance <= minObstacle) {
                brake();
            }
            delay(500);   //FIXME: Linus please check how far the car goes during a 500 delay. :)
            brake();
            atObstacle = tryTurning();
        }
    }
}

void handleInput(char input)
{ // Handle serial input if there is any

    switch (input)
    {

    case 'a': // Automatic driving
        driveWithAvoidance();
        break;

    case 'l': // Turn Left
        turnLeft(-turnAngle);
        break;

    case 'r': // Turn Right
        turnRight(turnAngle);
        break;

    case 'f': // Forward
        driveForward(speed);
        break;

    case 'b': // Backwards
        driveBackward(-speed);
        break;

    case 'i': // Increases carspeed by 10%
        car.setSpeed(car.getSpeed() + 10);
        break;

    case 'q': // Decreases carspeed by 10%
        car.setSpeed(car.getSpeed() - 10);
        break;

    default:
        brake();
    }
}

void readBluetooth()
{
    while (bluetooth.available())
    {
        char input = bluetooth.read();
        handleInput(input);
    }
}

void loop()
{
    readBluetooth(); //manual
}