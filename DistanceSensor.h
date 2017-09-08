/*
 * DistanceSensor.h
 *
 *  Created on: Aug 24, 2017
 *      Author: kevywilly
 */

#ifndef DISTANCESENSOR_H_
#define DISTANCESENSOR_H_

#include "Arduino.h"
#include "Tuples.h"

/*
 * Angle - Simple class for managing angles
 */
class Angle{
public:

	//////////////////////////////////////////////////////////////////////////////////////
	// Convert degrees to 360 (0-359) representation
	//////////////////////////////////////////////////////////////////////////////////////
	static int To360(int angle) {
		if(angle < 0)
			return angle+360.0;

		if(angle >=360)
			return angle - 360.0;

		return angle;
	}

	static tuple3<int> To360(tuple3<int> angles) {
			return {To360(angles._1), To360(angles._2), To360(angles._3)};
	}

	static int ToPlusMinus(int angle) {
			return (angle > 180) ? angle - 360 : angle;
	}

	static tuple3<int> ToPlusMinus(tuple3<int> angles) {
		return {ToPlusMinus(angles._1), ToPlusMinus(angles._2), ToPlusMinus(angles._3)};
	}

	static int MinAngleFromZero(int a, int b) {
		if(abs(ToPlusMinus(a)) < abs(ToPlusMinus(b)))
			return a;

		return b;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Is angle between to angles represented as 0-359 moving clockwise.
	//////////////////////////////////////////////////////////////////////////////////////
	static bool IsBetween(int left, int right, int angle) {

		int lft = To360(left);
		int target = To360(angle);
		int rgt = To360(right);

		int i = lft;
		while(i != rgt) {
			if(i == target)
				return true;
			i = To360(i+1);
		}
		return false;
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Obstacle: Spatial representation of an obstacle from center of viewer - distance, angle, arc  //
///////////////////////////////////////////////////////////////////////////////////////////////////
class Obstacle {
public:
	float radius;
	float theta;
	float thetaMin;
	float thetaMax;
	bool exists;
	char * key;

	// Generic constructor
	Obstacle() {
		radius = 0;
		theta = 0;
		thetaMin = 0;
		thetaMax = 0;
		exists = false;
		key = (char*) "none";
	}

	// Constructor
	Obstacle(char *_key, float _radius, float _theta, float _arc) {
		exists = true;
		radius = _radius;
		theta = Angle::To360(_theta);
		thetaMin = Angle::To360(theta - _arc);
		thetaMax = Angle::To360(theta + _arc);
		key = _key;
	}

	// Bool: Angle is blocked by an obstacle
	bool angleIsBlocked(int angle) {
		if(!exists)
			return false;

		return Angle::IsBetween(thetaMin, thetaMax, angle);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////
// Genric Distance Sensor - Abstract Class  										   //
/////////////////////////////////////////////////////////////////////////////////////////

class DistanceSensor {
public:

	uint16_t minDistance;
	uint16_t maxDistance;
	uint16_t threshold;
	uint16_t range;
	uint16_t currentDistance;
	float arc;
	float yaw;
	float x;
	float y;
	char * key;

	unsigned long timer = 0;

	DistanceSensor(char * _key, uint16_t _minDistance, uint16_t _maxDistance,
			uint16_t _threshold, uint16_t _range, float _x, float _y,
			float _yaw, float _arc) {
		key = _key;
		minDistance = _minDistance;
		maxDistance = _maxDistance;
		threshold = _threshold;
		range = _range;
		currentDistance = maxDistance;
		x = _x;
		y = _y;
		arc = _arc;
		yaw = _yaw;
	}

	virtual ~DistanceSensor() {

	}

	virtual void getDistance() = 0;


	bool tooClose() {
		return (currentDistance <= threshold);
	}

	bool inRange() {
		return (currentDistance <= range);
	}

	void pingIfReady(int ping_interval) {
		if (millis() >= timer) {
			timer += ping_interval;
			getDistance();
		}
	}

	Obstacle getObstacle() {
		if(!tooClose())
			return Obstacle();
		float y2 = sin(radians(yaw))/currentDistance;
		float x2 = cos(radians(yaw))/currentDistance;
		float xPos = x+x2;
		float yPos = y+y2;
		float radius = sqrt(xPos*xPos + yPos*yPos);
		float theta = degrees(asin(yPos/radius));
		return Obstacle(key, radius, theta, arc);
	}

};

#endif /* DISTANCESENSOR_H_ */
