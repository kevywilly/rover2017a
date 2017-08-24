/*
 * DistanceSensor.h
 *
 *  Created on: Aug 24, 2017
 *      Author: kevywilly
 */

#ifndef DISTANCESENSOR_H_
#define DISTANCESENSOR_H_


class DistanceSensor {
public:

	uint16_t minDistance;
	uint16_t maxDistance;
	uint16_t threshold;
	uint16_t range;
	uint16_t currentDistance;
	unsigned long timer = 0;

	DistanceSensor(uint16_t _minDistance, uint16_t _maxDistance, uint16_t _threshold, uint16_t _range) {
		minDistance = _minDistance;
		maxDistance = _maxDistance;
		threshold = _threshold;
		range = _range;
		currentDistance = maxDistance;
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
	        if(millis() >= timer) {
	          timer += ping_interval;
	          getDistance();
	        }
	    }
};


#endif /* DISTANCESENSOR_H_ */
