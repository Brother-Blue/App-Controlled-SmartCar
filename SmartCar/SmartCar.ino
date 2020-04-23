#include <Smartcar.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <VL53L0X.h>

BluetoothSerial bluetooth;

const int speed = 70;       // 70% of the full speed
const int turnAngle = 75;   // 75 Degrees to turn
const int minObstacle = 20; // Minimum distance ahead to obstacle
const int GYROSCOPE_OFFSET = 37;
unsigned int error = 0; //FIXME: Need to calculate error for LIDAR sensor readings.
unsigned int frontDistance;
unsigned int backDistance;
const unsigned int MAX_DISTANCE = 100; // Max distance to measure with ultrasonic

boolean atObstacle = false;

//FIXME: Make sure ultrasonic pins are correct for side or behind :)
const int TRIGGER_PIN = 5; // Trigger signal for ultrasonic
const int ECHO_PIN = 18;   // Reads signal for ultrasonic

const unsigned long PULSES_PER_METER = 600; // Amount of odometer pulses per 1 meter

SR04 back(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Ultrasonic sensor
VL53L0X frontSensor;                            // Micro LIDAR sensor

GY50 gyro(GYROSCOPE_OFFSET); // Gyroscope sensor

DirectionlessOdometer leftOdometer(
    smartcarlib::pins::v2::leftOdometerPin, []() { leftOdometer.update(); }, PULSES_PER_METER); // Odometer sensors
DirectionlessOdometer rightOdometer(
    smartcarlib::pins::v2::rightOdometerPin, []() { rightOdometer.update(); }, PULSES_PER_METER);

BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins); // Car motors
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

SmartCar car(control, gyro, leftOdometer, rightOdometer); // Initializing of the car

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    frontSensor.setTimeout(500);
    if (!frontSensor.init())
    {
        Serial.print("Failed to initialize VL53L0X sensor.");
        while (1)
        {
        }
    }

    frontSensor.startContinuous(100);
    bluetooth.begin("Group 2 SmartCar");
    Serial.print("Ready to connect!");
}

void turnRight(int degrees, int turnSpeed = speed) // Manual right turn
{
    car.setSpeed(turnSpeed);
    car.setAngle(degrees);
}

void turnLeft(int degrees, int turnSpeed = speed) // Manual left turn
{
    car.setSpeed(turnSpeed);
    car.setAngle(degrees);
}

void driveForward(int driveSpeed = speed) // Manual fordwards drive
{
    if (car.getSpeed() < 0)          // If car is currently going backward
    {                                // reverse slowly to fix motor overloading.
        while (car.getSpeed() < -10) // Using -10 as a threshold
        {
            car.setSpeed(car.getSpeed() + 10);
            delay(100); //FIXME: Check if reverse is to fast or slow
        }
    }
    car.setAngle(0);
    car.setSpeed(driveSpeed);
}

void driveBackward(int driveSpeed = -speed) // Manual backwards drive
{
    if (car.getSpeed() > 0)         // If car is currently going forward
    {                               // reverse slowly to fix motor overloading.
        while (car.getSpeed() > 10) // Using 10 as a threshold
        {
            car.setSpeed(car.getSpeed() - 10);
            delay(100); //FIXME: Check if reverse is to fast or slow
        }
    }
    car.setAngle(0);
    car.setSpeed(driveSpeed);
}

void brake() // Carstop
{
    car.setSpeed(0);
    car.setAngle(0);
}

void checkDistance() // Obstacle interference
{
    frontDistance = (frontSensor.readRangeSingleMillimeters() / 10) - error; // Divided by 10 to convert to cm
    backDistance = back.getDistance();
    if (frontSensor.timeoutOccurred())
    {
        Serial.print("VL53L0X sensor timeout occurred.");
    }

    if (frontDistance > 0 && frontDistance <= minObstacle)
    {
        atObstacle = true;
        brake();
    }
    if (backDistance > 0 && backDistance <= minObstacle)
    {
        brake();
    }
}

boolean tryTurning() // Try finding a new path around obstacle
{
    while (atObstacle)
    {
        // Try turning right
        for (int i = car.getHeading(); i < turnAngle; i += 5)
        {
            // Increment steering to the right and check if space is free
            turnRight(i, 20);
            delay(100);
            if (frontDistance > 85) // 85 was used as 100 is the max range for the LIDAR sensor
            {
                atObstacle = false;
                break;
            }
        }
        // Try turning left
        for (int i = car.getHeading(); i > -turnAngle; i -= 5)
        {
            // Increment steering to the right and check if space is free
            turnLeft(i, -20);
            delay(100);
            if (frontDistance > 85) // 85 was used as 100 is the max range for the LIDAR sensor
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
    if (!atObstacle)
    {
        checkDistance();
        driveForward(speed);
    }
    while (atObstacle && backDistance >= minObstacle)
    {
        driveBackward(-10);
        if (backDistance <= minObstacle)
        {
            brake();
        }
        delay(500); //FIXME: Linus please check how far the car goes during a 500 delay. :)
        brake();
        atObstacle = tryTurning();
    }
}

void handleInput(char input) // Handle serial input if there is any
{

    switch (input)
    {

    case 'a': // Automatic driving with obstacle avoidance
        driveWithAvoidance();
        break;

    case 'l': // Left turn
        turnLeft(-turnAngle);
        break;

    case 'r': // Right turn
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
    readBluetooth();
    checkDistance(); // Checks distance in manual mode.
}