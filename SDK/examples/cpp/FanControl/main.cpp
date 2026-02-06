/**
 * @example FanControl/main.cpp
 * @brief Fan/Fan control example.
 *
 * Demonstrates:
 * - Initializing FanControl
 * - setting fan speed
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "FanControl.h"
#include "Helper.h"

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

	// Initialize Tempreture Control
	// auto_control = false: we will control fan speed manually in this example
	// auto_control = true: automatic temperature control will try to manage CPU temperature under 70 degree Celsius	
	if (FanControl::init(false) < 0)
	{
		spdlog::error("FanControl init failed");
		return -2;
	}

	// get FanControl version
	spdlog::info("FanControl Version:{:.3f}", FanControl::getVersion());

	// Set fan speed to 100%
	FanControl::setFanSpeed(100);
	spdlog::info("Fan speed set to 100%");

	// Keep running for 7 seconds
	std::this_thread::sleep_for(std::chrono::seconds(7));

	// Turn off fan
	FanControl::setFanSpeed(0);
	spdlog::info("Fan turned off");

	// Cleanup
	FanControl::dispose();

	return 0;
}