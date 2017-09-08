/*
 * ir_sensor.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef IRSENSOR_H_
#define IRSENSOR_H_

#include "Arduino.h"
#include "DistanceSensor.h"


class IRSensor : public DistanceSensor{

public:
	IRSensor(char * key, uint8_t _sensor_pin, uint16_t _minVoltage, uint16_t _maxVoltage, uint16_t min_cm, uint16_t max_cm, int _threshold, int _range, float _x, float _y, float _angle, float _arc);
	void getDistance();

private:
	uint8_t sensor_pin;
	int16_t min_voltage;
	int16_t max_voltage;




};

#endif /* IRSENSOR_H_ */
