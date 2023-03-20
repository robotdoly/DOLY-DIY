#include "LcdControl.h"
#include <string.h>
#include <stdio.h>
#include <thread>

/// <summary>
/// This program demonstrates how to setup and control servos,
/// Also this program requires LcdControl library
/// LcdControl is part of Project Doly, libararies and headers are located under '/Doly'
/// Do not forget to Copy and Link related libraries.
/// </summary>

int FillBufferExample();

int main()
{
	// initialize Lcd 
	// default Color depth is 12 Bit (4096 color)
	LcdControl::init(LCD_12BIT);

	LcdColorDepth depth = LcdControl::getColorDepth();

	// Fill with buffer example
	FillBufferExample();

	// wait 2 seconds
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	// fill with color example
	LcdControl::LcdColorFill(LcdLeft, 0, 0, 255);
	LcdControl::LcdColorFill(LcdRight, 255, 0, 0);

	return 0;
}

int FillBufferExample()
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
	frame_left.side = LcdLeft;

	LcdData frame_right;
	frame_right.buffer = buf;
	frame_right.side = LcdRight;

	int retval = 0;
	// write buffer to LCD
	if (LcdControl::writeLcd(&frame_left) < 0)
	{
		printf("Left Lcd write failed! \n");
		retval = -1;
	}
	// write buffer to LCD
	if (LcdControl::writeLcd(&frame_right) < 0)
	{
		printf("Right Lcd write failed! \n");
		retval = -1;
	}

	return retval;
}