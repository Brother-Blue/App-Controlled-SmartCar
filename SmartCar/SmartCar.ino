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
const float SPEEDCHANGE = 0.1; // Used when increasing and decreasing speed. Might need a new more concrete name?

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

void rotate(int degrees, float speed)
{
    car.update();
    speed = smartcarlib::utils::getAbsolute(speed);
    degrees %= 360; // Put degrees in a (-360,360) scale
   
    car.setSpeed(speed);
    car.setAngle(degrees);

    const auto initialHeading    = car.getHeading();
    bool hasReachedTargetDegrees = false;
    while (!hasReachedTargetDegrees)
    {
        car.update();
        auto currentHeading = car.getHeading();
        if (degrees < 0 && currentHeading > initialHeading)
        {
            // If we are turning left and the current heading is larger than the
            // initial one (e.g. started at 10 degrees and now we are at 350), we need to substract
            // 360 so to eventually get a signed displacement from the initial heading (-20)
            currentHeading -= 360;
        }
        else if (degrees > 0 && currentHeading < initialHeading)
        {
            // If we are turning right and the heading is smaller than the
            // initial one (e.g. started at 350 degrees and now we are at 20), so to get a signed
            // displacement (+30)
            currentHeading += 360;
        }
        // Degrees turned so far is initial heading minus current (initial heading
        // is at least 0 and at most 360. To handle the "edge" cases we substracted or added 360 to
        // currentHeading)
        int degreesTurnedSoFar  = initialHeading - currentHeading;
        hasReachedTargetDegrees = smartcarlib::utils::getAbsolute(degreesTurnedSoFar)
                                  >= smartcarlib::utils::getAbsolute(degrees);
    }

    car.setSpeed(0);
}


void driveForward() // Manual forward drive
{
    car.setAngle(0);
    float currentSpeed = car.getSpeed();
    while(currentSpeed < SPEED){
        car.setSpeed(currentSpeed += SPEEDCHANGE);
    }
}


// Not yet used
void driveForwardDistance(long distance)
{
    long initialDistance = car.getDistance();
    long travelledDistance = 0;
    driveForward();
    
    while (travelledDistance <= distance)
    {
     car.update();
     long currentDistance = car.getDistance();
     travelledDistance = currentDistance - initialDistance;   
    }
    stopCar();
}


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
void stopCar()
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
        stopCar();
    }
    if (backDistance > 0 && backDistance <= MIN_OBSTACLE)
    {
        stopCar();
    }
}


// Manual drive inputs
void manualControl(char input)
{
    switch (input)
    {
    case 'l': // Left turn
        rotate(-TURN_ANGLE, SPEED);
        break;

    case 'r': // Right turn
        rotate(TURN_ANGLE, SPEED);
        break;

    case 'f': // Forward
        driveForward();
        break;

    case 'b': // Backwards
        driveBackward();
        break;

    case 'i': // Increases carspeed by 0.1
        car.setSpeed(car.getSpeed() + SPEEDCHANGE);
        break;

    case 'd': // Decreases carspeed by 0.1
        car.setSpeed(car.getSpeed() - SPEEDCHANGE);
        break;
        
    default:
        stopCar();
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