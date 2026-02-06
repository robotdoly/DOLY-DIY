/**
 * @example LedControl/main.cpp
 * @brief LED control usage example.
 *
 * Demonstrates:
 * - Initializing LedControl
 * - Controlling LED states and colors
 * - Receiving LED-related events
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "LedControl.h"
#include "Color.h"
#include "Helper.h"

void onLedComplete(uint16_t id, LedSide side)
{
	spdlog::info("Led Complete: ID:{} Side:{}", id, static_cast<uint8_t>(side));
}

void onLedError(uint16_t id, LedSide side, LedErrorType type)
{
	spdlog::error("Led Error: ID:{} Side:{} Type:{}", id, static_cast<uint8_t>(side), static_cast<uint8_t>(type));
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

	// Initialize LED Control
	if (LedControl::init() < 0)
	{
		spdlog::error("LedControl init failed");
		return -2;
	}

	// get LedControl version
	spdlog::info("LedControl Version:{:.3f}", LedControl::getVersion());

	// Add event listeners
	LedEvent::AddListenerOnComplete(onLedComplete);
	LedEvent::AddListenerOnError(onLedError);

	// Fade example
	LedActivity activity1;
	activity1.mainColor = Color::getColor(ColorCode::RED);
	activity1.fadeColor = Color::getColor(ColorCode::BLUE);
	activity1.fade_time = 2000; // 2 seconds

	LedControl::processActivity(1, LedSide::BOTH, activity1);

	// Let the activity run for a while on another thread
	std::this_thread::sleep_for(std::chrono::milliseconds(2500));

	// Change to solid color instantly
	LedActivity activity2;
	activity2.mainColor = Color::getColor(ColorCode::ORANGE);
	activity2.fade_time = 0; //

	LedControl::processActivity(2, LedSide::RIGHT, activity2);

	// Cleanup
	LedEvent::RemoveListenerOnComplete(onLedComplete);
	LedEvent::RemoveListenerOnError(onLedError);
	LedControl::dispose();

	return 0;
}