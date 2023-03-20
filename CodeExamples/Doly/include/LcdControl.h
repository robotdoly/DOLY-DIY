#pragma once
#include <stdint.h>
#include <vector>

#define LCD_WIDTH 240
#define LCD_HEIGHT 240

enum LcdColorDepth :uint8_t
{
	// 0x06 = 18 bit,  0x03 = 12 bit
	LCD_12BIT = 0x03,
	LCD_18BIT = 0x06,
};

enum LcdSide :uint8_t
{
	LcdLeft = 0,
	LcdRight = 1,
};

struct LcdData
{
	uint8_t side;
	uint8_t* buffer;
};

namespace LcdControl
{
	// Initialize lcd
	// return 0 success
	// return 1 already initialized
	// return -1 open device failed
	// return -2 ioctl failed
	int8_t init(LcdColorDepth depth = LCD_12BIT);

	// release file descriptor and deinitialize
	// return 0 success
	// return 1 already closed or not opened
	int8_t release();

	// retuns state of lcds
	// return true initialized
	// return false not initialized
	bool isActive();

	// fill lcd with RGB
	void LcdColorFill(LcdSide side, uint8_t R, uint8_t G, uint8_t B);

	// write buffer data to lcd
	// return 0 success
	// return -1 ioct error
	// return -2 not active
	int8_t writeLcd(LcdData* frame_data);

	//return lcd buffer size
	// pixel count * Color data byte size
	// ex. 240*240*3 
	int getBufferSize();

	// returns lcd color depth
	LcdColorDepth getColorDepth();

	// brightness value min = 0, max = 10
	int8_t setBrightness(uint8_t value);

	//converts 24 bit image to lcd image depth
	void LcdBufferFrom24Bit(uint8_t* output, uint8_t* input);
};

