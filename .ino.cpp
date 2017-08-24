#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-08-24 02:43:06

#include "Arduino.h"
#include "motors.h"
#include "motion.h"
#include "distance_sensor.h"
#include "sound.h"
#include "ir_sensor.h"
#include <Servo.h>
void setup() ;
void loop() ;
void echoCheck() ;
void pingIfReady() ;
int getTurnDir() ;
int turnToAvoid(int lft, int rgt) ;
void turnBestDirection(int power, int pct) ;
void spinBestDirection(int power) ;
bool allClear() ;
inline bool allTooClose() ;
inline bool irInRange() ;
inline bool irTooClose() ;
inline bool sonarTooClose() ;
inline bool sonarInRange() ;
inline void doAllClear() ;
inline void doIrInRange() ;
inline void doIrTooClose() ;
inline void doSonarInRange() ;
inline void doSonarTooClose() ;
inline void doReverse() ;
inline void doAllTooClose() ;
void move() ;
void init_neck() ;
void printDistances() ;

#include "rover2017a.ino"


#endif
