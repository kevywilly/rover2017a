
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

SonarSensor sonar((char*) "SONAR", SONAR_SERVO_PIN, SONAR_FRONT_TRIGGER, SONAR_FRONT_ECHO, 5,
		SONAR_MAX_DISTANCE, SONAR_THRESHOLD, SONAR_RANGE, 65.0, 0.0, 0.0, 45.0);

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

IRSensor irLeft((char*)"LEFT", IR_LEFT_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM,
		IR_THRESHOLD, IR_RANGE, 95.0, -35.0, 352.5, 30.0);
IRSensor irRight((char*)"RIGHT", IR_RIGHT_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM,
		IR_THRESHOLD, IR_RANGE, 95.0, 35.0, 7.5, 30.0);
IRSensor irRear((char*)"REAR", IR_REAR_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM,
		SONAR_THRESHOLD, SONAR_RANGE, 95.0, 0.0, 0.0, 30.0);

//////////////////////////////// DISTANCE SENSORS ////////////////////////////////////

const int kNumDistanceSensors = 3;
DistanceSensor * distanceSensors[] = { &sonar, &irLeft, &irRight }; //, &irRear };

///////////////////////////////////// SETUP /////////////////////////////////////////

void setup() {

#ifdef DEBUG
	Serial.begin(115200);
#endif

	randomSeed(analogRead(0));
	motors.stop();
	sonar.begin();

	//delay(1000);
}

///////////////////////////////////// MAIN LOOP /////////////////////////////////////////

void loop() {

	if (mode == CALIBRATION_MODE) {
		motors.setSpeed(0, 0);
		pingIfReady();
	} else {

		move();

	}
#ifdef DEBUG
	printDistances();
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
		if(obstacles[i].blocks(angle))
			return false;
	}
	return true;
}
int findClearAngle(Obstacle * obstacles) {

	if(angleIsClear(0, obstacles))
		return 0;

	for(int i=1; i < 180; i++) {
		if(angleIsClear(i,obstacles))
			return i;

		if(angleIsClear(360-i, obstacles))
			return (360-i);
	}

	return 90;
}
////////////////////////////////// MAIN DRIVE METHOD /////////////////////////////////////
void move() {

	pingIfReady();
	Obstacle obstacles[kNumDistanceSensors]; // = new Obstacle[4];
	for (int i = 0; i < kNumDistanceSensors; i++) {
		obstacles[i] = distanceSensors[i]->getObstacle();
    printObstacle(obstacles[i]);
	}

	int angle = findClearAngle(obstacles);

	if(angle > 180)
		angle -=360;

	if(angle == 0) {
		motors.forward(80);
	} else if(abs(angle) > 45) {
		motors.spinTo(angle, 60);
		motors.brake();
	} else {
		motors.turnToward(angle,60);
	}


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
