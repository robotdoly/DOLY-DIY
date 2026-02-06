
/**
 * @example LcdControl/main.cpp
 * @brief LCD control usage example.
 *
 * Demonstrates:
 * - Initializing LcdControl
 * - Controlling LCD backlight and display output
 */

#include <string.h>
#include <stdio.h>
#include <thread>
#include <spdlog/spdlog.h>

#include "Helper.h"
#include "LcdControl.h"

// this examples demonstrates basic usage of LcdControl LOW-LEVEL functions

int fillBufferExample();

int fillColorExample();

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

	// get LCD Control version
	spdlog::info("LcdControl Version:{:.3f}", LcdControl::getVersion());

	// initialize Lcd 
	// default Color depth is 12 Bit (4096 color)
	// 12 bit color uses less memory and is faster to write
	// use L18BIT for 18 Bit (262144 color), which has better color quality but slower to write
	if (LcdControl::init(LcdColorDepth::L12BIT) != 0) {
		spdlog::error("LcdControl init failed");
		return -2;
	}

	// set brightness 0 - 10
	LcdControl::setBrightness(10);

	// get current color depth
	LcdColorDepth depth = LcdControl::getColorDepth();
	spdlog::info("Lcd Color Depth: {}", (depth == LcdColorDepth::L12BIT) ? "12 Bit" : "18 Bit");

	// Demonstrates filling the LCD with a buffer
	fillBufferExample();

	// wait for 3 seconds before next example
	std::this_thread::sleep_for(std::chrono::seconds(3));

	// Demonstrates filling the LCD with a color
	fillColorExample();

	//dispose Lcd
	LcdControl::dispose();

	return 0;
}

int fillBufferExample()
{
	// get LCD buffer size 
	// buffer size depends on color depth
	// ex. Color depth 12 Bit frame size = 240px * 240px * 1.5
	// Color depth 18 Bit frame size = 240px * 240px * 3
	int buffer_size = LcdControl::getBufferSize();

	// allocate & clear buffer
	uint8_t buf[buffer_size];
	memset(&buf, 0xFF, buffer_size);

	// create data for each side
	LcdData frame_left;
	frame_left.buffer = buf;
	frame_left.side = LEFT;

	LcdData frame_right;
	frame_right.buffer = buf;
	frame_right.side = RIGHT;

	int retval = 0;
	// write buffer to LCD
	if (LcdControl::writeLcd(&frame_left) < 0)
	{
		spdlog::error("Left Lcd write failed! \n");
		retval = -1;
	}
	// write buffer to LCD
	if (LcdControl::writeLcd(&frame_right) < 0)
	{
		spdlog::error("Right Lcd write failed! \n");
		retval = -1;
	}

	return retval;
}

int fillColorExample()
{
	LcdControl::LcdColorFill(LEFT, 0, 0, 255);
	LcdControl::LcdColorFill(RIGHT, 255, 0, 0);

	return 0;
}
