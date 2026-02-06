/**
 * @example EyeControl/main.cpp
 * @brief EyeControl display and content example.
 *
 * Demonstrates:
 * - Initializing EyeControl
 * - Loading and displaying visual content
 * - Handling eye-related events
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include <Helper.h>
#include <EyeControl.h>
#include <EyeEvent.h>

void onEyeStart(uint16_t id)
{
	spdlog::info("Eye animation start id:{}", id);
}

void onEyeComplete(uint16_t id)
{
	spdlog::info("Eye animation complete id:{}", id);
}

void onEyeAbort(uint16_t id)
{
	spdlog::warn("Eye animation abort id:{}", id);
}

int main() {
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

	// Get EyeControl version
	spdlog::info("EyeControl Version:{:.3f}", EyeControl::getVersion());

	// Register event listeners
	EyeEvent::AddListenerOnComplete(onEyeComplete);
	EyeEvent::AddListenerOnAbort(onEyeAbort);
	EyeEvent::AddListenerOnStart(onEyeStart);

	// Initialize EyeControl 
	if (EyeControl::init(ColorCode::BLUE, ColorCode::WHITE) != 0) {
		spdlog::error("EyeControl init failed");
		return -2;
	}

	// Wait for a while before next example
	std::this_thread::sleep_for(std::chrono::seconds(3));

	// Example: Change iris
	EyeControl::setIris(IrisShape::MODERN, ColorCode::DARK_GREEN, EyeSide::BOTH);

	// Wait for a while before next example
	std::this_thread::sleep_for(std::chrono::seconds(3));

	// Example: Set Animation, animation works on different thread
	EyeControl::setAnimation(1, EyeExpressions::EXCITED);

	// wait until animation complete
	while (EyeControl::isAnimating())
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// Wait for a while before next example
	std::this_thread::sleep_for(std::chrono::seconds(3));

	// Example: Using custom images for lids, iris or background
	// create a visual content and load image 
	VContent visual = VContent::getImage("/.doly/images/lids/11B.png", true, true);
	if (!visual.isReady())
		spdlog::error("image load failed!");
	else
	{
		int ret = EyeControl::setLid(&visual, false, EyeSide::BOTH); // load content for bottom eyelid
		if (ret < 0)
			spdlog::error("Set eye lid failed err:{}", ret);
	}

	// Set eye position, scale iris, and set bottom lid position
	EyeControl::setPosition(EyeSide::BOTH, 120, 120, 1.2, 1.2, 0, 180);

	// Read iris position 
	int16_t x, y;
	EyeControl::getIrisPosition(EyeSide::LEFT, x, y);
	spdlog::info("Left iris postion: {},{}", x, y);


	// Cleanup
	EyeEvent::RemoveListenerOnComplete(onEyeComplete);
	EyeEvent::RemoveListenerOnAbort(onEyeAbort);
	EyeEvent::RemoveListenerOnStart(onEyeStart);

	return 0;
}
