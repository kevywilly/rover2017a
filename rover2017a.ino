
#include "Arduino.h"
#include "IRSensor.h"
#include "Motors.h"
#include "SonarSensor.h"
#include "DistanceSensor.h"

#define DEBUG

////////////////////////////// Operating Mode //////////////////////////////////

#define CALIBRATION_MODE 0
#define AUTONOMOUS_MODE 1

int mode = AUTONOMOUS_MODE;

//////////////////////////////// MOTOR SETTINGS ////////////////////////////////////

#define LEFT_MOTOR_PIN 11
#define RIGHT_MOTOR_PIN 12
#define LEFT_MOTOR_CENTER_MICROSECONDS 1547
#define RIGHT_MOTOR_CENTER_MICROSECONDS 1550
#define MAX_MOTOR_MICROSECONDS_RANGE 400
#define WHEEL_DIAMETER 9.5

Motors motors(LEFT_MOTOR_PIN, LEFT_MOTOR_CENTER_MICROSECONDS, RIGHT_MOTOR_PIN, RIGHT_MOTOR_CENTER_MICROSECONDS, MAX_MOTOR_MICROSECONDS_RANGE, WHEEL_DIAMETER);


////////////////////////////// Sonar Settings //////////////////////////////////

#define SONAR_FRONT_TRIGGER 5
#define SONAR_FRONT_ECHO 6
#define SONAR_MAX_DISTANCE 500
#define SONAR_SERVO_PIN 7
#define SONAR_THRESHOLD 25
#define SONAR_RANGE 50
#define SONAR_PING_INTERVAL 20
#define SONAR_X_POSITION 67.5 // 82.5-15
#define SONAR_Y_POSITION 0.0
#define SONAR_DETECTION_RANGE 45.0
#define SONAR_YAW 0.0

SonarSensor sonar((char*) "SONAR", SONAR_SERVO_PIN, SONAR_FRONT_TRIGGER, SONAR_FRONT_ECHO, 5,
		SONAR_MAX_DISTANCE, SONAR_THRESHOLD, SONAR_RANGE, SONAR_X_POSITION, SONAR_Y_POSITION, SONAR_YAW, SONAR_DETECTION_RANGE);

//////////////////////////////// IR Settings ////////////////////////////////////

#define IR_LEFT_PIN A0
#define IR_REAR_PIN A2
#define IR_RIGHT_PIN A1
#define IR_MIN_V 3
#define IR_MAX_V 600
#define IR_MIN_CM 4
#define IR_MAX_CM 30
#define IR_THRESHOLD 25
#define IR_RANGE 25
#define IR_PING_INTERVAL 15
#define IR_ARC 30

// Left IR Position
#define IR_LEFT_X 95.0
#define IR_LEFT_Y -35.0
#define IR_LEFT_YAW 348.75

// Right IR Position
#define IR_RIGHT_X 95.0
#define IR_RIGHT_Y 35.0
#define IR_RIGHT_YAW 11.25

// Rear IR Position
#define IR_REAR_X -90.0
#define IR_REAR_Y 0.0
#define IR_REAR_YAW 180.0

IRSensor irLeft((char*)"LEFT", IR_LEFT_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM,
		IR_THRESHOLD, IR_RANGE, IR_LEFT_X, IR_LEFT_Y, IR_LEFT_YAW, IR_ARC);
IRSensor irRight((char*)"RIGHT", IR_RIGHT_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM,
		IR_THRESHOLD, IR_RANGE, IR_RIGHT_X, IR_RIGHT_Y, IR_RIGHT_YAW, IR_ARC);
IRSensor irRear((char*)"REAR", IR_REAR_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM,
		SONAR_THRESHOLD, SONAR_RANGE, IR_REAR_X, IR_REAR_Y, IR_REAR_YAW, IR_ARC);

//////////////////////////////// DISTANCE SENSORS ////////////////////////////////////

const int kNumDistanceSensors = 3;
DistanceSensor * distanceSensors[] = { &sonar, &irLeft, &irRight }; //, &irRear };


//////////////////////// KEEP TRACK OF PREVIOUS HEADING WHEN TURNING /////////////////
int previousHeading = 0;

///////////////////////////////////// SETUP /////////////////////////////////////////

void setup() {

#ifdef DEBUG
	Serial.begin(115200);
#endif
	mode = AUTONOMOUS_MODE;
	randomSeed(analogRead(0));
	motors.stop();
	sonar.begin();

	//delay(1000);
}

///////////////////////////////////// MAIN LOOP /////////////////////////////////////////

void loop() {

	if (mode == CALIBRATION_MODE) {
		motors.stop();
		calibrate();
	} else {

		move();

	}
#ifdef DEBUG
	//printDistances();
#endif

}

////////////////////////////////// SONAR ECHO CHECK ////////////////////////////////////
void echoCheck() {
	sonar.echoCheck();
}

////////////////////////////////// PING DISTANCES /////////////////////////////////////
// Ping Sonars and IR if ping interval has been reached!
void pingIfReady() {
	sonar.pingIfReady(echoCheck, SONAR_PING_INTERVAL);
	irRear.pingIfReady(IR_PING_INTERVAL);
	irLeft.pingIfReady(IR_PING_INTERVAL);
	irRight.pingIfReady(IR_PING_INTERVAL);
}


int angleIsClear(int angle, Obstacle * obstacles) {
	for(int i=0; i < kNumDistanceSensors; i++) {
		if(obstacles[i].angleIsBlocked(angle))
			return false;
	}
	return true;
}

tuple3<int> findClearAngle(Obstacle * obstacles) {
	bool foundLeft = false;
	bool foundRight = false;
	int right = 0;
	int left = 0;

	if(angleIsClear(0, obstacles))
		return tuple3<int>{0,0,0};

	for(int i=1; i < 180; i++) {
		if(!foundRight && angleIsClear(i,obstacles)) {
			right = i;
			foundRight = true;
		}

		if(!foundLeft && angleIsClear(360-i, obstacles)){
			left = (360-i);
			foundLeft = true;
		}

		if(foundLeft && foundRight) {
			return tuple3<int>{Angle::MinAngleFromZero(left, right), left, right};
		}
	}

	return tuple3<int>{90,270,90};
}
////////////////////////////////// MAIN CALIBRATION METHOD ///////////////////////////////

void calibrate() {

		pingIfReady();
    bool hasObstacle = false;
		Obstacle obstacles[kNumDistanceSensors]; // = new Obstacle[4];
		for (int i = 0; i < kNumDistanceSensors; i++) {
			obstacles[i] = distanceSensors[i]->getObstacle();
      if(obstacles[i].exists) {
	      printObstacle(obstacles[i]);
        printDistances();
        hasObstacle = true;
      }
		}

		if(hasObstacle) {
			tuple3<int> angles = findClearAngle(obstacles);

			if(Serial) {
				Serial.println("Clear Angles: ");
				Serial.print("best: ");
				Serial.print(angles._1);
				Serial.print(" left: ");
				Serial.print(angles._2);
				Serial.print(" right: ");
				Serial.print(angles._3);
			}
		}
   delay(5000);
}




////////////////////////////////// MAIN DRIVE METHOD /////////////////////////////////////
void move() {
	bool hasObstacle = false;
	pingIfReady();

	Obstacle obstacles[kNumDistanceSensors]; // = new Obstacle[4];
	for (int i = 0; i < kNumDistanceSensors; i++) {
		obstacles[i] = distanceSensors[i]->getObstacle();
    if(obstacles[i].exists) {
        printObstacle(obstacles[i]);
        printDistances();
        hasObstacle = true;
      }
	}

	if(!hasObstacle) {
		motors.forward(70);
		previousHeading = 0;
	}

	tuple3<int> angles = Angle::ToPlusMinus(findClearAngle(obstacles));

	int angle = angles._1;

	// Keep turning same direction if turning again
	/*
	if(angle != 0) {
		if(previousHeading < 0 && abs(angles._2 - previousHeading) < 2) { // turning left
			angle = previousHeading;
		} else if(abs(angles._3 - previousHeading) < 2) { // turning right
			angle = previousHeading;
		}
	}*/
	if(previousHeading !=0 && angle !=0) {
		return;
	}

	if(angle == 0) {
		motors.forward(70);
	} else if(abs(angle) > 45) {
		motors.spinTo(angle, 90);
		motors.brake();
	} else {
		motors.turnToward(angle,60);
	}

	previousHeading = angle;
}

////////////////////////////////// SERIAL PRINT METHODS /////////////////////////////////////
void printDistances() {
	if (!Serial)
		return;

	String result = irLeft.currentDistance + String(":")
			+ sonar.currentDistance + String(":") + irRight.currentDistance;
	Serial.println(result);

}

void printObstacle(Obstacle obstacle) {
	if (!obstacle.exists)
		return;
	if (!Serial)
		return;

	Serial.println("------------------- Obstacle ----------------");
  Serial.print(obstacle.key);
	Serial.print(": Radius: "); Serial.print(obstacle.radius);
	Serial.print(" Theta: "); Serial.print(obstacle.theta);
  Serial.print(" Range: "); Serial.print(obstacle.thetaMin);
	Serial.print(" - "); Serial.println(obstacle.thetaMax);

}
