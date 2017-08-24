/*
 * motion.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef MOTION_H_
#define MOTION_H_

class MotionSensor {
  public:

    MotionSensor(uint8_t pin) {
      sensor_pin = pin;
      pinMode(pin, INPUT);
      digitalWrite(pin, LOW);
    }

    bool detected() {
      return (digitalRead(sensor_pin) > 0);
    }

  private:
    uint8_t sensor_pin;

};

#endif /* MOTION_H_ */
