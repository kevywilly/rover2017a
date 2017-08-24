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


    IRSensor(uint8_t _sensor_pin, uint16_t _minVoltage, uint16_t _maxVoltage, uint16_t min_cm, uint16_t max_cm, int _threshold, int _range) :
    	DistanceSensor(min_cm, max_cm, _threshold, _range) {
    	sensor_pin = _sensor_pin;
    	min_voltage = _minVoltage;
    	max_voltage = _maxVoltage;
    	pinMode(sensor_pin, INPUT);

    }

    void getDistance() {
       currentDistance = map(analogRead(sensor_pin), min_voltage, max_voltage, maxDistance, minDistance);
    }

  private:
    uint8_t sensor_pin;
    int16_t min_voltage;
    int16_t max_voltage;




};

#endif /* IRSENSOR_H_ */
