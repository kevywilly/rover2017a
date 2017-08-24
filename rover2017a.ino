#include "motors.h"
#include "motion.h"
#include "distance_sensor.h"
#include "sound.h"
#include "ir_sensor.h"
#include <Servo.h>

#define DEBUG

#define NECK_PIN 10
#define SONAR_FRONT_TRIGGER 5
#define SONAR_FRONT_ECHO 6
#define SONAR_MAX_DISTANCE 500

#define IR_LEFT_PIN A0
#define IR_REAR_PIN A1
#define IR_RIGHT_PIN A2

#define IR_MIN_V 3
#define IR_MAX_V 600
#define IR_MIN_CM 4
#define IR_MAX_CM 30
#define IR_TOO_CLOSE 25
#define IR_IN_RANGE 25
#define IR_PING_INTERVAL 20

#define SERVO_LEFT_PIN 11
#define SERVO_RIGHT_PIN 12
#define SERVO_LEFT_CENTER_MICROS 1545
#define SERVO_RIGHT_CENTER_MICROS 1545
#define MOTOR_MAX_POWER_OFFSET 220

#define SONAR_TOO_CLOSE 25
#define SONAR_IN_RANGE 50
#define SONAR_PING_INTERVAL 40

#define CALIBRATION_MODE 0
#define AUTONOMOUS_MODE 1

IRSensor ir_left(IR_LEFT_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM, IR_TOO_CLOSE, IR_IN_RANGE);
IRSensor ir_right(IR_RIGHT_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM, IR_TOO_CLOSE, IR_IN_RANGE);
IRSensor ir_rear(IR_REAR_PIN, IR_MIN_V, IR_MAX_V, IR_MIN_CM, IR_MAX_CM, SONAR_TOO_CLOSE, SONAR_IN_RANGE);

Servo neck;
DistanceSensor sonar(SONAR_FRONT_TRIGGER, SONAR_FRONT_ECHO, SONAR_MAX_DISTANCE, SONAR_TOO_CLOSE, SONAR_IN_RANGE);
Motors motors(SERVO_LEFT_PIN,SERVO_LEFT_CENTER_MICROS,SERVO_RIGHT_PIN,SERVO_RIGHT_CENTER_MICROS,MOTOR_MAX_POWER_OFFSET);

//NeuralNetwork Brain;

int sonar_angles[3] = {135,45,90};
int mode = AUTONOMOUS_MODE;


typedef struct {
  long left;
  long front;
  long right;
  long rear;
  MotorState state;
} CurrentState;

void setup() {

#ifdef DEBUG
  Serial.begin(115200);
#endif

  randomSeed(analogRead(0));

  motors.stop();
  //motors.setSpeed(0,0);
  init_neck();
  delay(1000);
}

void loop() {

  if(mode == CALIBRATION_MODE){
    motors.setSpeed(0,0);
    pingIfReady();
  } else {
     pingIfReady();
     move();


  }
   #ifdef DEBUG
      printDistances();
    #endif


}


void echoCheck() {
  sonar.echoCheck();
}

// Ping Sonars and IR if ping interval has been reached!
void pingIfReady() {
  sonar.pingIfReady(echoCheck, SONAR_PING_INTERVAL);
  ir_rear.pingIfReady(IR_PING_INTERVAL);
  ir_left.pingIfReady(IR_PING_INTERVAL);
  ir_right.pingIfReady(IR_PING_INTERVAL);
}

// Get Turn direction based on ir_left, ir_right
int getTurnDir() {
  return (ir_left.cm < ir_right.cm) ? 1 : -1;
}


int turnToAvoid(int lft, int rgt) {
  int r = 100*float(lft-4.0)/(float(lft-4.0)+float(rgt-4.0));

  motors.turn(100-r, r);
  return r;
}

void turnBestDirection(int power, int pct) {
  int dir = ir_left.cm < ir_right.cm ? 1 : -1;

  if(dir < 0) {
    motors.turn(int(power*pct/100.0), power);
  } else {
    motors.turn(power, int(power*pct/100.0));
  }

}

void spinBestDirection(int power) {
  int dir = ir_left.cm < ir_right.cm ? 1 : -1;

  motors.spin(power*(ir_left.cm < ir_right.cm ? 1 : -1));

}
// No sonars or IR's are too close
bool allClear() {
    return(!sonar.isInRange() && !ir_right.isInRange() && !ir_left.isInRange());
}

inline bool allTooClose() {
    return(sonar.isTooClose() && ir_left.isTooClose() && ir_right.isTooClose());
}

inline bool irInRange() {
  return (ir_left.isInRange() || ir_right.isInRange());
}
inline bool irTooClose() {
  return (ir_left.isTooClose() || ir_right.isTooClose());
}

inline bool sonarTooClose() {
  return (sonar.isTooClose()); // || ir_front.isTooClose());
}

inline bool sonarInRange() {
  return sonar.isInRange();
}

inline void doAllClear() {
  motors.forward(100);
}
inline void doIrInRange() {
    turnToAvoid(ir_left.cm,ir_right.cm);
    //turnBestDirection(80,60);
}

inline void doIrTooClose() {
    turnToAvoid(ir_left.cm,ir_right.cm);
    //turnBestDirection(80,20);
}

inline void doSonarInRange() {
   motors.forward(80);
}

inline void doSonarTooClose() {
  spinBestDirection(80);
}

inline void doReverse() {
  motors.reverse(80);
}

inline void doAllTooClose() {
  doReverse();
}
void move() {

 if(allTooClose()) {
    doAllTooClose();
    return;
  }


  if(allClear() && (!motors.status || MotorState::Reverse)) {
    doAllClear();
    return;
  }

  switch (motors.status) {

    // Already moving forward
    case MotorState::Forward:
      if (sonarTooClose()) {
          // check sonar too close
          doSonarTooClose();
      } else if(irTooClose()) {
          // check irs too clos
          doIrTooClose();
      } else if(irInRange()) {
          // check ir in range
          doIrInRange();
      }
      else if (sonarInRange()) {
          // check sonr in range
          doSonarInRange();
      }
      else {
        motors.forward(100);
      }
      break;

    // Already SPINNING
    case MotorState::Spinning:
      if(!sonarTooClose()) {
        // Go back to forward if sonar no longer too close
        motors.forward(80);
      }
      break;

    case MotorState::Turning:
      if (sonarTooClose()) {
        // if sonar is too close, then spin!
        doSonarTooClose();
      } else if(irInRange()) {
        // if ir in range and no longer too close then accomodate
        doIrInRange();
      }
      break;

     case MotorState::Reverse:
        if(!sonarTooClose()) {
          spinBestDirection(80);
          delay(500);
        }

      break;
    default:
      doAllClear();
  }

    //buzzer.silent();
}

void init_neck() {
  neck.attach(NECK_PIN);
  neck.write(100);
  //neck.detach();
}

void printDistances() {
#ifdef DEBUG
  String result = ir_left.cm + String(":") + sonar.cm + String(":") + ir_right.cm;
  Serial.println(result);
#endif
}
