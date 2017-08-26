/*
 * Motors.cpp
 *
 *  Created on: Aug 26, 2017
 *      Author: kevywilly
 */

#include "Motors.h"

Motors::Motors(uint8_t left_servo_pin, uint16_t left_servo_zero_micros, uint8_t right_servo_pin, uint16_t right_servo_zero_micros,
			uint16_t servo_range_micros, float wheel_diameter) {

		status = MotorStatus::Stopped;
		left_servo_pin_ = left_servo_pin;
		right_servo_pin_ = right_servo_pin;
		left_servo_center_micros_ = left_servo_zero_micros;
		right_servo_center_micros_ = right_servo_zero_micros;
		servo_range_micros_ = servo_range_micros;
		last_power_left_ = 0;
		last_power_right_ = 0;

		delay_360_ = 1000*(1.0/(servo_range_micros_/360.0));
		wheel_diameter_ = wheel_diameter;
		wheel_circumference_ = PI * wheel_diameter_;

		randomSeed(analogRead(0));
	}


	// Attach motors
	void Motors::attach() {
		servo_left_.attach(left_servo_pin_, left_servo_center_micros_ - servo_range_micros_/2,
				right_servo_center_micros_ + servo_range_micros_/2);
		servo_right_.attach(right_servo_pin_, right_servo_center_micros_ - servo_range_micros_/2,
				left_servo_center_micros_ + servo_range_micros_/2);
	}

	void Motors::detach() {
		servo_left_.detach();
		servo_right_.detach();
	}

	void Motors::stop() {
		brake();
		detach();
		status = MotorStatus::Stopped;
	}

	// Set speed for motors
	void Motors::setSpeed(int8_t p_lft, int8_t p_rgt) {

		attach();
		last_power_left_ = p_lft;
		last_power_right_ = p_rgt;

		int16_t l = power_to_micros(-p_lft, left_servo_center_micros_);
		int16_t r = power_to_micros(p_rgt, right_servo_center_micros_);
		servo_left_.writeMicroseconds(l);
		servo_right_.writeMicroseconds(r);

	}

	int Motors::getDelayForDistance(float distance, int power) {
		return round((abs(distance) / wheel_circumference_) * delay_360_ * (100.0 / power));
	}
	int Motors::getDelayForAngle(int angle, int power) {
		return round(map(abs(angle), 0, 360, 0, delay_360_) * 100.0 / power);
	}

	// move to relative x,y coordinate
	void Motors::moveToward(float x, float y, int8_t power) {
		float dist = Position2D::calcDistance(x, y);
		float theta = Position2D::calcTheta(x, y);
		spinToward(theta, power);
		moveTowardX(dist, power);
	}

	// move to relative x,y coordinate
	void Motors::moveTo(float x, float y, int8_t power) {
		moveToward(x, y, power);
		stop();
	}

	void Motors::moveToX(float x, int8_t power) {
		moveTowardX(x, power);
		stop();
	}

	void Motors::moveTowardX(float x, int8_t power) {
		if (x != 0)
			(x < 0) ? forward(-power) : forward(power);

		delay(getDelayForDistance(x, power));
	}
	// Spin to an angle
	void Motors::spinTo(int angle, int8_t power) {
		spinToward(angle, power);
	}

	void Motors::spinToward(int angle, int8_t power) {
		if (angle != 0)
			(angle < 0) ? turn(-power, power) : turn(power, -power);

		delay(getDelayForAngle(angle, power));
	}

	// Turn to an angle
	void Motors::turnTo(int angle, int8_t power) {
		turnToward(angle, power);
		stop();
	}

	void Motors::turnToward(int angle, int8_t power) {

		if (angle != 0)
			(angle < 0) ? turn(0, power) : turn(power, 0);

		delay(getDelayForAngle(angle, power) * 2);
	}

	void Motors::brake() {
		uint8_t powL = last_power_left_;
		uint8_t powR = last_power_right_;

		while (powL != 0 && powR != 0) {
			powL -= (powL - 0) * 0.66;
			powR -= (powR - 0) * 0.66;
			if (abs(powL) < 20)
				powL = 0;

			if (abs(powR) < 20)
				last_power_right_ = 0;

			setSpeed(powL, powR);
			delay(5);
		}

		status = MotorStatus::Braking;
	}

	void Motors::forward(int8_t power) {
		setSpeed(power, power);
		status = MotorStatus::Forward;
	}

	void Motors::reverse(int8_t power) {
		setSpeed(-power, -power);
		status = MotorStatus::Reverse;
	}

	void Motors::move(int8_t p_left, int8_t p_right) {
		setSpeed(p_left, p_right);
		if (p_left != p_right) {
			status = MotorStatus::Turning;
		} else if (p_left == p_right) {
			if (p_left < 0)
				status = MotorStatus::Braking;
			else if (p_left == 0)
				status = MotorStatus::Reverse;
			else
				status = MotorStatus::Forward;
		}
	}

	void Motors::turn(int8_t p_left, int8_t p_right) {
		setSpeed(p_left, p_right);

		status = MotorStatus::Turning;
	}

	void Motors::spin(int8_t power) {
		setSpeed(power, -power);
		status = MotorStatus::Spinning;
	}
