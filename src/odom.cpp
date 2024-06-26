#include "vex.h"
#include "odom.h"
#include "robot-config.h"

using namespace vex;

// constants
const double wheelDiameter = 3.0; // inches
const double wheelCircumference = 3.141592653589793 * wheelDiameter;

// GLOBAL COORDINATES
double Odom::globalX = 0.0;
double Odom::globalY = 0.0;
double Odom::globalAngle = 0.0;
double Odom::prevGlobalX = 0.0;
double Odom::prevGlobalY = 0.0;

// LOCAL COORDINATES
Point Odom::localDeltaPoint = {0, 0};

// SENSOR VALUES
// motor values
double Odom::leftfWheelPos = LFMotor.position(degrees);
double Odom::rightfWheelPos = RFMotor.position(degrees);// Separate right back wheel motor
// angle
double Odom::currentAngle = imu.heading(degrees);
double Odom::prevAngle = 0.0;

double Odom::prevLeftfWheelPos = 0.0;
double Odom::prevRightfWheelPos = 0.0;

double Odom::deltaAngle = currentAngle - prevAngle;
// ODOMETRY FUNCTIONS
void Odom::updateSensors() {
  leftfWheelPos = LFMotor.position(degrees);
  rightfWheelPos = RFMotor.position(degrees);
  currentAngle = imu.heading(degrees);

  // Replace encoder values with motor values
  double leftMotorDelta = leftfWheelPos - prevLeftfWheelPos;
  double rightMotorDelta = rightfWheelPos - prevRightfWheelPos;

  // Update angle
  prevAngle = currentAngle;
  deltaAngle = currentAngle - prevAngle;
  if (deltaAngle > 180) deltaAngle -= 360;
  if (deltaAngle < -180) deltaAngle += 360;

  // Update motor values
  double leftWheelDelta = leftMotorDelta * (wheelCircumference / 360.0);
  double rightWheelDelta = rightMotorDelta * (wheelCircumference / 360.0);

  // Polar coordinates
  localDeltaPoint.x = (rightWheelDelta + leftWheelDelta) / 2.0;
  localDeltaPoint.y = (rightWheelDelta - leftWheelDelta) / 2.0;

  // Cartesian coordinates
  //globalX += (localDeltaPoint.y * sin(prevAngle + deltaAngle / 2)) + (localDeltaPoint.x * cos(prevAngle + deltaAngle / 2));
  //globalY += (localDeltaPoint.y * cos(prevAngle + deltaAngle / 2)) - (localDeltaPoint.x * sin(prevAngle + deltaAngle / 2));
  globalX += localDeltaPoint.x * cos(globalAngle) - localDeltaPoint.y * sin(globalAngle);
  globalY += localDeltaPoint.x * sin(globalAngle) + localDeltaPoint.y * cos(globalAngle);

  globalAngle = currentAngle;

  // Update previous motor values
  prevLeftfWheelPos = leftfWheelPos;
  prevRightfWheelPos = rightfWheelPos;
}

void Odom::reset() {
  leftfWheelPos = 0.0;
  rightfWheelPos = 0.0;
  prevAngle = globalAngle;
  prevGlobalX = globalX;
  prevGlobalY = globalY;
}

void Odom::setPosition(double newX, double newY, double newAngle) {
  reset();
  prevAngle = newAngle;
  prevGlobalX = newX;
  prevGlobalY = newY;
}

// ODOMETRY THREAD
int Odom::Odometry() {
  while (true) {

    Odom::updateSensors();
    // Print or use globalX, globalY, globalAngle as needed

    Brain.Screen.clearScreen();
    Brain.Screen.clearLine();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print(Odom::globalX);
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print(Odom::globalY);
    Brain.Screen.setCursor(3, 1);
    Brain.Screen.print(imu.heading(degrees));
    this_thread::sleep_for(10);
  }
  return 0;
}
