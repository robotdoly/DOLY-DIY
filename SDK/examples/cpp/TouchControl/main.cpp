/**
 * @example TouchControl/main.cpp
 * @brief Touch sensor usage example.
 *
 * Demonstrates:
 * - Initializing TouchControl
 * - Receiving touch events
 * - Handling touch input callbacks
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "TouchControl.h"
#include "Helper.h"

void onTouchEvent(TouchSide side, TouchState state)
{
	spdlog::info("Touch Event Side: {} State: {}", static_cast<uint8_t>(side), static_cast<uint8_t>(state));
}

void onTouchActivityEvent(TouchSide side, TouchActivity activity)
{
	spdlog::info("Touch Activity Event Side: {} Activity: {}", static_cast<uint8_t>(side), static_cast<uint8_t>(activity));
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

	// Initialize Touch Control
	if (TouchControl::init() < 0)
	{
		spdlog::error("TouchControl init failed");
		return -2;
	}

	// get TouchControl version
	spdlog::info("TouchControl Version:{:.3f}", TouchControl::getVersion());

	// Add event listener for touch state changes and activities
	TouchEvent::AddListenerOnTouch(onTouchEvent);
	TouchEvent::AddListenerOnTouchActivity(onTouchActivityEvent);

	// Run for 30 seconds to test sensors
	std::this_thread::sleep_for(std::chrono::seconds(30));

	// Cleanup
	TouchEvent::RemoveListenerOnTouch(onTouchEvent);
	TouchEvent::RemoveListenerOnTouchActivity(onTouchActivityEvent);
	TouchControl::dispose();

	return 0;
}