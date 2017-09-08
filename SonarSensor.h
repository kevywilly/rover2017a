/*
 * distance_sensor.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef SONARSENSOR_H_
#define SONARSENSOR_H_

#include <Servo.h>
#include <NewPing.h>
#include "DistanceSensor.h"

class SonarSensor : public DistanceSensor{

  public :

	// Center Angle for sonar
	static const uint8_t kServoCenter = 90;

    // Use NewPing Library for Sonar
    NewPing sensor;

	// Constructor
    SonarSensor(char * key, uint8_t _servo_pin, uint8_t trigger, uint8_t echo, uint16_t _minDistance, uint16_t _maxDistance, uint16_t _threshold, uint16_t _range, float _x, float _y, float _angle, float _arc) :
    	DistanceSensor(key, _minDistance, _maxDistance, _threshold, _range, _x, _y, _angle, _arc), sensor(trigger, echo, _maxDistance){
      servo_pin_ = _servo_pin;
      trigger_pin_ = trigger;
      echo_pin_ = echo;

    }

    // Attach the servo and write center
    void begin() {
      servo.attach(servo_pin_);
      servo.write(kServoCenter);
    }

    // Trigger ping timer
    bool pingIfReady(void (*userFunc)(void), int ping_interval) {
      if(millis() >= timer) {
          timer = millis() + ping_interval;
          sensor.timer_stop();
          sensor.ping_timer(userFunc, maxDistance);
          return true;
      } else {
        return false;
      }
    }

    // Set distance 0 converted to max distance
    void setDistance(long d) {
      currentDistance = (d <= 0) ? maxDistance : d;
    }

    // Read Distance without ping timer
    void getDistance() {
    	setDistance(sensor.ping_cm(maxDistance));
    }

    // Echo check method for ping timer
    void echoCheck() {
        if(sensor.check_timer()) {
          setDistance(NewPing::convert_cm(sensor.ping_result));
        }
    }


   private:

    Servo servo;
    uint8_t servo_pin_;
    uint8_t trigger_pin_;
    uint8_t echo_pin_;


};

#endif /* SONARSENSOR_H_ */
