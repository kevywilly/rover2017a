/*
 * motors.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include <Servo.h>

typedef enum {
    Forward,Spinning,Turning,Braking,Reverse,Stopped
} MotorState;

typedef enum {
  left=-1, right=0
} Dir;

class Motors {
    public:
      Motors(uint8_t l_pin, uint16_t l_zero, uint8_t r_pin, uint16_t r_zero, uint8_t max_pow) {
         lft_pin = l_pin;
         rgt_pin = r_pin;
         lft_zero = l_zero;
         rgt_zero = r_zero;
         max_power = max_pow;

         randomSeed(analogRead(0));
      }

      static const int8_t DIR_LFT = -1;
      static const int8_t DIR_RGT = 1;

      MotorState status = MotorState::Stopped;

      uint8_t max_power;

      // Attach motors
      void attach() {
          lft.attach(lft_pin, lft_zero-max_power, rgt_zero+max_power);
          rgt.attach(rgt_pin, rgt_zero-max_power, lft_zero+max_power);
      }

      void detach() {
        lft.detach();
        rgt.detach();
      }

      void stop() {
        detach();
        status = MotorState::Stopped;
      }


      // Set speed for motors
      void setSpeed(int8_t p_lft, int8_t p_rgt) {

        attach();
        int16_t l = powerToMicros(-p_lft, lft_zero);
        int16_t r = powerToMicros(p_rgt, rgt_zero);
        lft.writeMicroseconds(l);
        rgt.writeMicroseconds(r);

      }

      void brake() {
        setSpeed(0,0);
        status = MotorState::Braking;
      }

      void forward(int8_t power) {
        setSpeed(power, power);
        status = MotorState::Forward;
      }

      void reverse(int8_t power) {
        setSpeed(-power, -power);
        status = MotorState::Reverse;
      }
      void turn(int8_t p_left, int8_t p_right) {
          setSpeed(p_left, p_right);

          status = MotorState::Turning;
      }

      void spin(int8_t power) {
        setSpeed(power, -power);
        status = MotorState::Spinning;
      }

      void spinRandom(int8_t power) {
        spin(power*randomDir());
      }

    private:
      uint8_t lft_pin;
      uint8_t rgt_pin;
      uint16_t lft_zero;
      uint16_t rgt_zero;

      Servo lft;
      Servo rgt;

      // Convert power level 0 - 100% to microseconds
      int16_t powerToMicros(int8_t power, int16_t zero) {

        int16_t pow = map(abs(power), 0, 100, 0, max_power);
        if (power < 0) {
          pow = pow * -1;
        }
        //Serial.println(zero+pow);
        return (zero + pow);

      }

      int randomDir() {
        if (random(1, 100) < 51) {
          return -1;
        } else {
          return 1;
        }
      }


};

#endif /* MOTORS_H_ */
