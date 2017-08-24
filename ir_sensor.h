/*
 * ir_sensor.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef IR_SENSOR_H_
#define IR_SENSOR_H_



#define MIN_VOLTAGE 3
#define MAX_VOLTAGE 580
#define MIN_DISTANCE 4
#define MAX_DISTANCE 30
#ifndef IR_TOO_CLOSE
  #define IR_TO_CLOSE 15
#endif
#ifndef IR_IN_RANGE
  #define IR_IN_RANGE 20
#endif

class IRSensor {
  public:
    IRSensor(uint8_t pin) {
      init(pin, MIN_VOLTAGE, MAX_VOLTAGE, MIN_DISTANCE, MAX_DISTANCE, IR_TO_CLOSE, IR_IN_RANGE);
    }

    IRSensor(uint8_t pin, uint16_t min_v, uint16_t max_v, uint16_t min_cm, uint16_t max_cm, int tooclose, int inrange) {
       init(pin, min_v, max_v, min_cm, max_cm, tooclose, inrange);
    }

    long cm = MAX_DISTANCE;

    void getDistance() {
       cm = map(analogRead(sensor_pin), min_voltage, max_voltage, max_distance, min_distance);
    }

    bool isTooClose() {
      return (cm <= too_close);
    }

    bool isInRange() {
      return (cm <= in_range);
    }

    void pingIfReady(int ping_interval) {
        if(millis() >= timer) {
          timer += ping_interval;
          getDistance();
        }
    }

  private:
    uint8_t sensor_pin;
    int16_t min_voltage = MIN_VOLTAGE;
    int16_t max_voltage = MAX_VOLTAGE;
    int16_t min_distance = MIN_DISTANCE;
    int16_t max_distance = MAX_DISTANCE;
    int too_close;
    int in_range;
    unsigned long timer = 0;

    void init(uint8_t pin, uint16_t min_v, uint16_t max_v, uint16_t min_cm, uint16_t max_cm, int tooclose, int inrange) {
      sensor_pin = pin;
      min_voltage = min_v;
      max_voltage = max_v;
      min_distance = min_cm;
      max_distance = max_cm;
      too_close = tooclose;
      in_range = inrange;
      pinMode(sensor_pin, INPUT);
    }
};

#endif /* IR_SENSOR_H_ */
