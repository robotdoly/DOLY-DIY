/**
 * @example ServoControl/main.cpp
 * @brief ServoControl usage example.
 *
 * Demonstrates:
 * - Initializing ServoControl
 * - Controlling servo positions
 * - Handling servo-related events
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "ServoControl.h"
#include "Helper.h"

void onServoAbort(uint16_t id, ServoId channel)
{
	spdlog::info("Servo abort id:{} channel:{}", id, static_cast<uint8_t>(channel));
}
void onServoError(uint16_t id, ServoId channel)
{
	spdlog::error("Servo error id:{} channel:{}", id, static_cast<uint8_t>(channel));
}
void onServoComplete(uint16_t id, ServoId channel)
{
	spdlog::info("Servo completed id:{} channel:{}", id, static_cast<uint8_t>(channel));
}


int main()
{
	// Setup spdlog
	spdlog::set_level(spdlog::level::info); // Set as needed
	spdlog::flush_on(spdlog::level::trace); // flush everything

	// *** IMPORTANT *** 
	// Stop doly service if running,
	// otherwise instance of libraries cause conflict	
	if (Helper::stopDolyService() < 0) {
		spdlog::error("Doly service stop failed");
		return -1;
	}

	// Initialize Servo Control
	if (ServoControl::init() < 0)
	{
		spdlog::error("ServoControl init failed");
		return -2;
	}

	// Get ServoControl version
	spdlog::info("ServoControl Version:{:.3f}", ServoControl::getVersion());

	// Add event listeners
	ServoEvent::AddListenerOnComplete(onServoComplete);
	ServoEvent::AddListenerOnError(onServoError);
	ServoEvent::AddListenerOnAbort(onServoAbort);

	// Test servos
	ServoControl::setServo(1, ServoId::SERVO_0, 180, 100, false);
	ServoControl::setServo(2, ServoId::SERVO_1, 180, 1, false);

	std::this_thread::sleep_for(std::chrono::seconds(3));

	// Optional release function; disables servo hold position
	ServoControl::release(ServoId::SERVO_0);
	ServoControl::release(ServoId::SERVO_1);

	// Cleanup
	ServoEvent::RemoveListenerOnComplete(onServoComplete);
	ServoEvent::RemoveListenerOnError(onServoError);
	ServoEvent::RemoveListenerOnAbort(onServoAbort);
	ServoControl::dispose();

	return 0;
}