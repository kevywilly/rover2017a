/*
 * IRSensor.cpp
 *
 *  Created on: Aug 26, 2017
 *      Author: kevywilly
 */

#include "IRSensor.h"


////////////////////////////////////
// Constructor
////////////////////////////////////
 IRSensor::IRSensor(char * key, uint8_t _sensor_pin, uint16_t _minVoltage, uint16_t _maxVoltage, uint16_t min_cm, uint16_t max_cm, int _threshold, int _range, float _x, float _y, float _angle, float _arc) :
    	DistanceSensor(key, min_cm, max_cm, _threshold, _range,_x, _y, _angle, _arc) {
    	sensor_pin = _sensor_pin;
    	min_voltage = _minVoltage;
    	max_voltage = _maxVoltage;
    	pinMode(sensor_pin, INPUT);

    }

////////////////////////////////////
// Get distance to obstacle
////////////////////////////////////
void IRSensor::getDistance() {
   currentDistance = map(analogRead(sensor_pin), min_voltage, max_voltage, maxDistance, minDistance);
}
