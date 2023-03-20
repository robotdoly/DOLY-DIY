#pragma once
#include <stdint.h>

// just for information, ignored if used more than defined (220 degree)
// Arm servos can not rotate more than that.
#define SERVO_ARM_MAX_ANGLE 220

enum ServoChannel : uint8_t
{
	SERVO_LEFT,
	SERVO_RIGHT,
	SERVO_0,
	SERVO_1,
};

namespace ServoMotor
{
	// initialize all motors
	void Init();

	//return true if module ready
	bool isActive();

	// setup servo default values
	// 'min_us, max_us' depends on your servo and effects angle of movement
	// most PWM range them between (500us - 2500us)
	// 'angle' ignored for SERVO_LEFT & SERVO_RIGHT,
	// return 0 success
	// return -1 frequency interval error
	// return -2 angle error
	int8_t setup(ServoChannel channel, uint16_t min_us, uint16_t max_us, uint16_t angle, bool invert);

	// sets servo position
	// return 0 success
	// return -1 max angle exceed error
	// return -2 speed range error (0-100)
	// return -3 undefined channel
	int8_t set(ServoChannel channel, float angle, uint8_t speed);

	// stops servo
	// return 0 success
	// return -1 undefined servo
	int8_t stop(ServoChannel channel);
};

