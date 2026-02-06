/**
 * @example TofControl/main.cpp
 * @brief Time-of-Flight sensor usage example.
 *
 * Demonstrates:
 * - Initializing TofControl
 * - Receiving distance measurements
 * - Handling ToF sensor events
 */

#include <spdlog/spdlog.h>
#include <thread>
#include "TofControl.h"
#include "Helper.h"

void onProximityGesture(TofGesture left, TofGesture right)
{
	spdlog::info("TOF GESTURE L[{}] R[{}] ", static_cast<int>(left.type), static_cast<int>(right.type));
}
void onProximityThreshold(TofData left, TofData right)
{
	spdlog::warn("TOF PROXIMITY L[{}] R[{}] ", static_cast<int>(left.range_mm), static_cast<int>(right.range_mm));
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

	// Initialize ToF Control
	if (TofControl::init() < 0)
	{
		spdlog::error("TofControl init failed");
		return -2;
	}

	// Setup continues ToF Control with 50ms interval and proximity threshold 40mm (enabled)
	if (TofControl::setup_continuous(50, 40) < 0)
	{
		spdlog::error("TofControl setup failed");
		TofControl::dispose();
		return -3;
	}

	// Get TofControl version
	spdlog::info("TofControl Version:{:.3f}", TofControl::getVersion());

	// Add event listener for touch state changes and activities
	TofEvent::AddListenerOnProximityThreshold(onProximityThreshold);
	TofEvent::AddListenerOnProximityGesture(onProximityGesture);

	// Run for 30 seconds to test sensors
	std::this_thread::sleep_for(std::chrono::seconds(30));

	// Stop continuous reading and disable events
	TofControl::setup_continuous(0);

	// Single read example
	std::vector<TofData> sensors = TofControl::getSensorsData(); 
	spdlog::info("Single Read Sensor[{}] Range[{}mm] Error[{}]", static_cast<int>(sensors[0].side), sensors[0].range_mm, static_cast<int>(sensors[0].error));
	spdlog::info("Single Read Sensor[{}] Range[{}mm] Error[{}]", static_cast<int>(sensors[1].side), sensors[1].range_mm, static_cast<int>(sensors[1].error));

	// Cleanup
	TofEvent::RemoveListenerOnProximityThreshold(onProximityThreshold);
	TofEvent::RemoveListenerOnProximityGesture(onProximityGesture);
	TofControl::dispose();

	return 0;
}