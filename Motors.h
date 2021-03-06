/*
 * motors.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef MOTORS_H_
#define MOTORS_H_


#include "Arduino.h"
#include "Servo.h"

typedef enum {
	Forward, Spinning, Turning, Braking, Reverse, Stopped
} MotorStatus;


class Position2D {
public:
	float xPos;
	float yPos;

	Position2D(float x, float y) {
		xPos = x;
		yPos = y;
	}

	static float calcTheta(float x, float y) {
		return degrees(asin(x/sqrt(x*x + y*y)));
	}

	static float calcDistance(float x, float y) {
		return sqrt(x*x + y*y);
	}

};

class Motors {
public:

	MotorStatus status;


	Motors(uint8_t left_servo_pin, uint16_t left_servo_center_micros, uint8_t right_servo_pin, uint16_t right_servo_center_micros,
			uint16_t servo_range_micros, float wheel_diameter);

	// Attach motors
	void attach();
	void detach();
	void stop();
	void brake();

	// Set speed for motors
	void setSpeed(int8_t p_lft, int8_t p_rgt);

	void moveToward(float x, float y, int8_t power);
	void move(int8_t p_left, int8_t p_right);

	int getDelayForDistance(float distance, int power);
	int getDelayForAngle(int angle, int power);

	// Basic movement
	void forward(int8_t power);
	void reverse(int8_t power);
	void turn(int8_t p_left, int8_t p_right);
	void spin(int8_t power);

	// Turn to an angle
	void turnTo(int angle, int8_t power);
	void turnToward(int angle, int8_t power);
	// move to relative x,y coordinate
	void moveTo(float x, float y, int8_t power);
	void moveToX(float x, int8_t power);
	void moveTowardX(float x, int8_t power);
	// Spin to an angle
	void spinTo(int angle, int8_t power);
	inline void spinToward(int angle, int8_t power);

private:
	uint8_t left_servo_pin_;
	uint8_t right_servo_pin_;
	uint16_t left_servo_center_micros_;
	uint16_t right_servo_center_micros_;

	uint8_t last_power_left_;
	uint8_t last_power_right_;

	int delay_360_;
	float wheel_diameter_;
	float wheel_circumference_;

	Servo servo_left_;
	Servo servo_right_;

	uint16_t servo_range_micros_;

	// Convert power level 0 - 100% to microseconds
	int16_t power_to_micros(int8_t power, int16_t zero);

	int randomDir();

};

#endif /* MOTORS_H_ */
