/*
 * servo_arrow.h
 *
 *  Created on: Apr 16, 2016
 *      Author: crackoff
 */

#ifndef INCLUDE_SERVO_ARROW_H_
#define INCLUDE_SERVO_ARROW_H_

#define SERVO_PIN 4
#define PWM_CYCLE_DUTY_US 20000
#define PWM_STEPS_COUNT 20

void init_servo();
void arrow_set_position_us(unsigned ultime);
void arrow_set_position_by_time(unsigned time);

#endif /* INCLUDE_SERVO_ARROW_H_ */
