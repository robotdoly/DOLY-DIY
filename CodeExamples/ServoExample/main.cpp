#include "ServoMotor.h"
#include "Gpio.h"
#include <thread>

/// <summary>
/// This program demonstrates how to setup and control servos,
/// Also this program requires pthread, ServoMotor, Gpio, Timer libraries
/// ServoMotor, Gpio, Timer are part of Project Doly, libararies and headers are located under '/Doly'
/// Do not forget to Copy and Link related libraries.
/// </summary>

int main()
{
	// Setup Servo power control GPIO as output and set HIGH as a default value
	GPIO::init(Pin_Servo_Left_Enable, GPIO_OUTPUT, HIGH);
	GPIO::init(Pin_Servo_Right_Enable, GPIO_OUTPUT, HIGH);

	// Initialize and configure servo motors
	ServoMotor::Init();
	// Setup needs channel, bandwitdh range, max angle and working direction parameters
	// Setup ignores the 'max angle' parameter for both arm servos, however they are already defined internally.
	ServoMotor::setup(SERVO_LEFT, 500, 2500, SERVO_ARM_MAX_ANGLE, false);
	ServoMotor::setup(SERVO_RIGHT, 500, 2500, SERVO_ARM_MAX_ANGLE, true);

	// Sets servo zero positions
	// The 'speed' paramater range between 1-100
	ServoMotor::set(SERVO_LEFT, 0, 100);
	ServoMotor::set(SERVO_RIGHT, 0, 100);

	// wait to get zero position
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));


	// Sets servo positions
	// The 'speed' paramater range between 1-100
	ServoMotor::set(SERVO_LEFT, 90, 1);
	ServoMotor::set(SERVO_RIGHT, 90, 1);

	// wait
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	ServoMotor::set(SERVO_LEFT, SERVO_ARM_MAX_ANGLE, 100);
	ServoMotor::set(SERVO_RIGHT, SERVO_ARM_MAX_ANGLE, 100);

	// wait servo
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));

	// disable servo power
	GPIO::writePin(Pin_Servo_Left_Enable, LOW);
	GPIO::writePin(Pin_Servo_Right_Enable, LOW);

	return 0;
}