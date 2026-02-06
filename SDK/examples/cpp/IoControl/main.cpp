/**
 * @example IoControl/main.cpp
 * @brief IO control usage example.
 *
 * Demonstrates:
 * - Initializing IoControl
 * - Controlling digital and analog IO
 * - Handling IO-related events
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "IoControl.h"
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

	// Get IoControl version
	spdlog::info("IoControl Version:{:.3f}", IoControl::getVersion());

	// INFORMATION
	spdlog::info("Before testing, connect IO port 0 and port 1 (pin_0 <-> pin_1).");

	// write pin state => HIGH 
	uint8_t io_0 = 0; // IO port pin_0
	IoControl::writePin(1, 0, GpioState::HIGH);
	// read IO pin state
	uint8_t io_1 = 1; // IO port pin_1
	GpioState state = IoControl::readPin(2, io_1);
	spdlog::info("Read Pin:{} State:{}", io_1, static_cast<uint8_t>(state));

	// write pin state => LOW 
	IoControl::writePin(1, 0, GpioState::LOW);
	// read IO pin state
	state = IoControl::readPin(2, io_1);
	spdlog::info("Read Pin:{} State:{}", io_1, static_cast<uint8_t>(state));

	return 0;
}