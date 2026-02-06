/**
 * @example EdgeControl/main.cpp
 * @brief EdgeControl usage example.
 *
 * Demonstrates:
 * - Initializing EdgeControl
 * - Receiving edge sensor events
 * - Handling edge detection callbacks
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "EdgeControl.h"
#include "Helper.h"


void onEdgeChange(std::vector<IrSensor> sensors)
{
	spdlog::info("Edge Change Detected:");
	for (IrSensor sensor : sensors)
	{
		spdlog::info("Sensor id: {} state: {}", static_cast<uint8_t>(sensor.id), static_cast<uint8_t>(sensor.state));
	}
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

	// Initialize Edge Control
	if (EdgeControl::init() < 0)
	{
		spdlog::error("EdgeControl init failed");
		return -2;
	}

	// get EdgeControl version
	spdlog::info("EdgeControl Version:{:.3f}", EdgeControl::getVersion());

	// Add event listener for edge changes
	EdgeEvent::AddListenerOnChange(onEdgeChange);

	// Run for 20 seconds to test edge detection
	std::this_thread::sleep_for(std::chrono::seconds(20));

	// Cleanup
	EdgeEvent::RemoveListenerOnChange(onEdgeChange);
	EdgeControl::dispose();

	return 0;
}