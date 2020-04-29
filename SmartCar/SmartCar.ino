#include <Smartcar.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <VL53L0X.h>

BluetoothSerial bluetooth;

// Constansts
const float SPEED = 1.0;        // 1 m/s speed
const int TURN_ANGLE = 90;   // 90 Degrees to turn
const int MIN_OBSTACLE = 20; // Minimum distance ahead to obstacle
const int GYROSCOPE_OFFSET = 48;
const unsigned int MAX_DISTANCE = 100; // Max distance to measure with ultrasonic
const float SPEEDCHANGE = 0.2; // Used when increasing and decreasing speed. Might need a new more concrete name?

// Unsigned
//unsigned int error = 0; //FIXME: VAFN ÄR DET HÄR OCH VARFÖR BEHÖVER VI DET?!?!
unsigned int frontDistance;
unsigned int backDistance;

// Boolean
boolean atObstacle = false;

// Ultrasonic trigger pins
const int TRIGGER_PIN = 5; // Trigger signal
const int ECHO_PIN = 18;   // Reads signal

// Sensor pins
SR04 back(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Ultrasonic
VL53L0X frontSensor;                            // Micro LIDAR
GY50 gyro(GYROSCOPE_OFFSET);                    // Gyroscope

// Odometer
const unsigned long PULSES_PER_METER = 600; // TODO CALIBRATE PULSES ON CAR
DirectionlessOdometer leftOdometer(
    smartcarlib::pins::v2::leftOdometerPin, []() { leftOdometer.update(); }, PULSES_PER_METER);
DirectionlessOdometer rightOdometer(
    smartcarlib::pins::v2::rightOdometerPin, []() { rightOdometer.update(); }, PULSES_PER_METER);

// Car motors
BrushedMotor leftMotor(smartcarlib::pins::v2::leftMotorPins);
BrushedMotor rightMotor(smartcarlib::pins::v2::rightMotorPins);
DifferentialControl control(leftMotor, rightMotor);

// Car initializing
SmartCar car(control, gyro, leftOdometer, rightOdometer);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    car.enableCruiseControl(); // enabelCruiseControl example(2, 0.1, 0.5, 80) f f f l
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

/*
// Manual right turn
void turnRight(unsigned int degrees = TURN_ANGLE, int turnSpeed = car.getSpeed())
{
    if (turnSpeed == 0)
        turnSpeed = 10;
    car.setAngle(degrees);
    car.setSpeed(turnSpeed);
}
*/

/*
// Auto right turn
void turnRightInPlace(unsigned int degrees = TURN_ANGLE, unsigned int turnSpeed = car.getSpeed())
{
    if (turnSpeed == 0)
        turnSpeed = 20;
    gyro.update(); // Get current heading and save it.
    int curPos = gyro.getHeading();
    int targetPos = degrees + curPos; // Calculate new heading and normalize it to [0-360).
    if (targetPos > 360)
        targetPos = targetPos - 360;
    leftMotor.setSpeed(turnSpeed);   // Invert motors to turn car in place. Left motors must turn
    rightMotor.setSpeed(-turnSpeed); // forward while right goes backward in order to turn right
    while (gyro.getHeading() < targetPos - 3 || gyro.getHeading() > targetPos + 3)
    {
        gyro.update();
    }
    brake();
    gyro.update();
}
*/

/*
// Manual left turn
void turnLeft(int degrees = -TURN_ANGLE, int turnSpeed = car.getSpeed()) // Manual left turn
{
    if (degrees > 0)
        degrees = -degrees;
    if (turnSpeed == 0)
        turnSpeed = 10; //FIXME: Check how much car moves with speed 10
    car.setAngle(degrees);
    car.setSpeed(turnSpeed);
}
*/

/* Auto left turn
void turnLeftInPlace(int degrees = -TURN_ANGLE, unsigned int turnSpeed = car.getSpeed())
{
    if (turnSpeed == 0)
        turnSpeed = 20;
    gyro.update(); // Get current heading and save it.
    int curPos = gyro.getHeading();
    int targetPos = degrees + curPos; // Calculate new heading and normalize it to [0-360).
    if (targetPos < 0)
        targetPos = 360 + targetPos;
    leftMotor.setSpeed(-turnSpeed); // Invert motors to turn car in place. Right motors must turn
    rightMotor.setSpeed(turnSpeed); // forward while left goes backward in order to turn left
    while (gyro.getHeading() < targetPos - 3 || gyro.getHeading() > targetPos + 3)
    {
        gyro.update();
    }
    brake();
    gyro.update();
}
*/

void driveForward() // Manual forward drive
{
    car.setAngle(0);
    float currentSpeed = car.getSpeed();
    while(currentSpeed < SPEED){
        car.setSpeed(currentSpeed += SPEEDCHANGE);
    }
}

/*
// Not yet used
void driveForwardDistance(unsigned int driveSpeed = SPEED, unsigned int distance = 1)
{
    int cur = 0;
    leftOdometer.reset();
    rightOdometer.reset();
    driveForward(driveSpeed);
    while (cur < distance)
    {
        cur = car.getDistance();
    }
    brake();
}
*/

void driveBackward() // Manual backwards drive
{
    car.setAngle(0);
    float currentSpeed = car.getSpeed();
    while(currentSpeed > -SPEED){
        car.setSpeed(currentSpeed -= SPEEDCHANGE);
    }
}


/*
// Auto drive backwards
void driveBackwardDistance(int driveSpeed = -SPEED, unsigned int distance = 1)
{
    if (driveSpeed > 0)
        driveSpeed = -driveSpeed;
    int cur = 0;
    leftOdometer.reset();
    rightOdometer.reset();
    driveBackward(driveSpeed);
    while (cur < distance)
    {
        cur = car.getDistance();
    }
    brake();
}
*/

// Carstop
void brake()
{
    car.setSpeed(0);
    car.setAngle(0);
}


// Obstacle interference
void checkDistance()
{
    frontDistance = frontSensor.readRangeSingleMillimeters(); // Divided by 10 to convert to cm
    backDistance = back.getDistance();
    if (frontSensor.timeoutOccurred())
    {
        Serial.print("VL53L0X sensor timeout occurred.");
    }
    if (frontDistance > 0 && frontDistance <= MIN_OBSTACLE)
    {
        atObstacle = true;
        brake();
    }
    if (backDistance > 0 && backDistance <= MIN_OBSTACLE)
    {
        brake();
    }
}

/*
// Tries to avoid obstacle
void avoidObstacle()
{
    driveBackwardDistance(50);
    atObstacle = false;
    turnRightInPlace(90, 30); // Turn right 90 degrees
    checkDistance();          // Recheck if there's an obstacle in front, if not return false.
    if (atObstacle)
    {
        turnLeftInPlace(180, 30); // Turn 180 degrees to the left
        atObstacle = false;
        checkDistance();
        if (atObstacle)
            turnRightInPlace(90, 30); // Align to original position.
    }
    driveWithAvoidance();
}
*/
 /*
// Drive until obstacle
void driveWithAvoidance()
{
    if (atObstacle) // While you're at an obstacle
    {
        avoidObstacle();
    }
    else
    {
        driveForward();
    }
    while (!atObstacle) // While you're not at an obstacle
    {
        checkDistance();
    }
}
*/

/*
// Choose which way the car is controlled
void carControl()
{
    Serial.print("m = manualControl, a = driveWithAvoidance");
    char input = readBluetooth();

    switch (input)
    {
    case 'm':
        manualControl(input);
        break;

    case 'a':
        //driveWithAvoidance();
        break;
    }
}
*/

// Manual drive inputs
void manualControl(char input)
{
    switch (input)
    {
    case 'l': // Left turn
        car.setSpeed(SPEED);
        car.setAngle(-TURN_ANGLE);
        break;

    case 'r': // Right turn
        car.setSpeed(SPEED);
        car.setAngle(TURN_ANGLE);
        break;

    case 'f': // Forward
        driveForward();
        break;

    case 'b': // Backwards
        driveBackward();
        break;

    case 'i': // Increases carspeed by 0.2
        car.setSpeed(car.getSpeed() + SPEEDCHANGE);
        break;

    case 'd': // Decreases carspeed by 0.2
        car.setSpeed(car.getSpeed() - SPEEDCHANGE);
        break;

    default:
        brake();
    }
}

// Bluetooth inputs
void readBluetooth(){
  while(bluetooth.available()){
        char msg = bluetooth.read();
        manualControl(msg);
    }
}

void loop()
{
    readBluetooth();
    car.update();
    float curSpeed = car.getSpeed(); 
    Serial.println(curSpeed);
}