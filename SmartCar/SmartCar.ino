#include <Smartcar.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <VL53L0X.h>

BluetoothSerial bluetooth;

// Constansts
const float SPEED = 0.7;        // Speed in m/s 
const int TURN_ANGLE = 90;   // 90 Degrees to turn forward
const int BACK_MIN_OBSTACLE = 30; // Minimum distance for SR04
const int FRONT_MIN_OBSTACLE = 300; // Minimum distance for Micro-LIDAR
const int GYROSCOPE_OFFSET = 13;
const unsigned int MAX_DISTANCE = 100; // Max distance to measure with ultrasonic
const float SPEEDCHANGE = 0.1; // Used when increasing and decreasing speed. Might need a new more concrete name?

// Unsigned
unsigned int backSensorError = 3;
unsigned int frontSensorError = 30; 
unsigned int frontDistance;
unsigned int backDistance;

// Boolean
boolean atObstacle = false;
boolean autoDrivingEnabled = false;

// Ultrasonic trigger pins
const int TRIGGER_PIN = 5; // Trigger signal
const int ECHO_PIN = 18;   // Reads signal

// Sensor pins
SR04 backSensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Ultrasonic measures in centimeters
VL53L0X frontSensor;                            // Micro LIDAR measures in millimeters
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
    //speed = smartcarlib::utils::getAbsolute(speed);
    degrees %= 360; // Put degrees in a (-360,360) scale
   
    car.setSpeed(speed);
    if(speed < 0)
    {
        if(degrees > 0)
        {
            car.setAngle(-TURN_ANGLE);
        } 
        else 
        {
            car.setAngle(TURN_ANGLE);
        }
        
    } else {
        if(degrees > 0)
        {
            car.setAngle(TURN_ANGLE);
        } 
        else 
        {
            car.setAngle(-TURN_ANGLE);
        
        }
    }

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
        hasReachedTargetDegrees = smartcarlib::utils::getAbsolute(degreesTurnedSoFar) >= smartcarlib::utils::getAbsolute(degrees); 
    }
    car.setSpeed(0);
}


void driveForward() // Manual forward drive
{
    car.setAngle(0);
    car.update();
    float currentSpeed = car.getSpeed();
    while(currentSpeed < SPEED){
        car.setSpeed(currentSpeed += SPEEDCHANGE);
    }
}


// Not yet used
void driveDistance(long distance, int speed)
{
    long initialDistance = car.getDistance();
    long travelledDistance = 0;

    if(speed > 0)
    {
        driveForward();
    } else {
        driveBackward();
    }
    
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
    car.update();
    float currentSpeed = car.getSpeed();
    while(currentSpeed > -SPEED)
    {
        car.setSpeed(currentSpeed -= SPEEDCHANGE);
    }
}

// Carstop
void stopCar()
{
    car.setSpeed(0);
    car.setAngle(0);
}


// Obstacle interference
void stopAtObstacle()
{
    atObstacle = false;
    frontDistance = (frontSensor.readRangeSingleMillimeters() - frontSensorError);
    backDistance = (backSensor.getDistance() - backSensorError);
    if (frontSensor.timeoutOccurred())
    {
        Serial.print("VL53L0X sensor timeout occurred.");
    }
    if (frontDistance > 0 && frontDistance <= FRONT_MIN_OBSTACLE)
    {
        atObstacle = true;
        stopCar();
    }
    if (backDistance > 0 && backDistance <= BACK_MIN_OBSTACLE)
    {
        atObstacle = true;
        stopCar();
    }
}

void findPath()
{
}

void automatedDriving()
{
    while (autoDrivingEnabled)
    {
        driveForward();
        stopAtObstacle();
        if (atObstacle)
        {
            findPath();
        }
    }
    
}

void driveOption(char input)
{
    switch(input)
    {
        case 'a':
            autoDrivingEnabled = true;
            break;

        case 'm':
            autoDrivingEnabled = false;
            break;
    }
}
// Manual drive inputs
void manualControl(char input)
{
    switch (input)
    {

    case 'a':
        automatedDriving();
        break;

    case 'l': // Left turn
        rotate(-TURN_ANGLE, SPEED);
        break;

    case 'r': // Right turn
        rotate(TURN_ANGLE, SPEED);
        break;

    case 'k':
        rotate(TURN_ANGLE, -SPEED); // left backwards turn
        break;
    
    case 'j':
        rotate(-TURN_ANGLE, -SPEED); // right backwards turn
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

    case 'c': // Drive forward a set distance
        driveDistance(100, SPEED );
        break;
        
    case 'p': // Drive backwards a set distance
        driveDistance(100, -SPEED );
        break;
    
        
    default:
        stopCar();
    }
}

// Bluetooth inputs
void readBluetooth(){
  while(bluetooth.available()){
        char msg = bluetooth.read();
        //driveOption(msg);
        manualControl(msg);
    }
}

void loop()
{
    
    //autoDrivingEnabled = true;
    //automatedDriving();
    
    readBluetooth();
    car.update();
    
    
    /*
    if(autoDrivingEnabled)
    {
        automatedDriving();
    }*/

}