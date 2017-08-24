/*
 * distance_sensor.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef DISTANCE_SENSOR_H_
#define DISTANCE_SENSOR_H_

#include <NewPing.h>

class DistanceSensor {

  public :
    DistanceSensor(uint8_t trigger, uint8_t echo, uint16_t max, uint16_t tc, uint16_t ir) : sensor(trigger, echo, max){
      max_distance = max;
      trigger_pin = trigger;
      echo_pin = echo;
      cm = max_distance;
      too_close = tc;
      in_range = ir;

    }

    NewPing sensor;
    unsigned long timer = 0;

    long cm = 500;
    long too_close;
    long in_range;

    bool pingIfReady(void (*userFunc)(void), int ping_interval) {
      if(millis() >= timer) {
          timer = millis() + ping_interval;
          sensor.timer_stop();
          sensor.ping_timer(userFunc, max_distance);
          return true;
      } else {
        return false;
      }
    }

    void setDistance(long d) {
      cm = (d <= 0) ? max_distance : d;
    }

    void echoCheck() {
        if(sensor.check_timer()) {
          setDistance(NewPing::convert_cm(sensor.ping_result));
        }
    }


/*
    long ping() {

      pinMode(echo_pin, OUTPUT);
      pinMode(trigger_pin, OUTPUT);
      pinMode(echo_pin, INPUT);
      digitalWrite(trigger_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigger_pin, HIGH);
      delayMicroseconds(5);
      digitalWrite(trigger_pin, LOW);

      return pulseIn(echo_pin, HIGH);

    }
*/

  inline bool isTooClose() {

      return (cm <= too_close);
    }

  inline bool isInRange() {
      return (cm <= in_range);
    }

   private:


    long max_distance;
    uint8_t trigger_pin;
    uint8_t echo_pin;


};

#endif /* DISTANCE_SENSOR_H_ */
