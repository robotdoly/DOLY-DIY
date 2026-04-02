/**
 * @example BatteryControl/main.cpp
 * @brief BatteryControl basic usage example.
 *
 * Demonstrates:
 * - Initializing Battery
 * - Subscribing to BatteryEvent
 * - Reading simple battery information
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include <BatteryControl.h>
#include <BatteryEvent.h>
#include <Helper.h>

void BatteryAlarm(uint8_t capacity)
{
	spdlog::info("Battery ALARM !! capacity: {}", capacity);
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

	// get DriveControl version
	spdlog::info("BatterControl Version:{:.3f}", BatteryControl::getVersion());

	// Register event listeners
	BatteryEvent::AddListenerOnAlarm(BatteryAlarm);

	// Initialize Battery module
	BatteryControl::init();

	// Set alarm threshold to 20% 
	BatteryControl::SetAlarmThreshold(20);



	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		// Read and log current battery capacity
		uint8_t capacity = BatteryControl::getCapacity();
		spdlog::info("Battery capacity: {}", capacity);
	}

	return 0;
}