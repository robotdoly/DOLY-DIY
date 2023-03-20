#pragma once
#include <stdint.h>

enum GpioType :uint8_t
{
	GPIO_INPUT = 0,
	GPIO_OUTPUT = 1,
	GPIO_PWM = 2
};

enum GpioState :bool
{
	LOW = false,
	HIGH = true,
};

enum PinId :uint8_t
{
	// >= 50 GPIO_CHIP2
	Pin_Servo_Left_Enable = 56,
	Pin_Servo_Right_Enable = 57,
};

enum PwmId :uint8_t
{
	Pwm_Led_Left_B = 6,
	Pwm_Led_Left_G = 7,
	Pwm_Led_Left_R = 8,
	Pwm_Led_Right_B = 9,
	Pwm_Led_Right_G = 10,
	Pwm_Led_Right_R = 11,
};

namespace GPIO
{
	// Initialize IO pin
	// state is optional for GPIO
	// return 0 = success
	// return -1 = initialized failed
	// return -2 = wrong type
	int8_t init(PinId id, GpioType type, GpioState state = GpioState::LOW);

	// Initialize PWM pin
	// return 0 = success
	// return -1 = initialized failed
	int8_t init(PwmId id);

	// return 0 success
// return -1 write failed
// return -2 undefined id
	int8_t writePin(PinId id, GpioState state);

	// return 0 success
	// return -1 write failed
	// return -2 undefined id
	int8_t writePwm(PwmId id, uint16_t value);

};
