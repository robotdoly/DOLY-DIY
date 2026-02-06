#pragma once
#include <stdint.h>
#include <vector>

/**
 * @file LcdControl.h
 * @brief Low-level LCD control API for Doly displays.
 *
 * This header provides a minimal, low-level interface for initializing the LCD device(s),
 * writing frame buffers, and controlling brightness.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Use init() before calling writeLcd(), setBrightness(), or buffer conversion helpers
 * - Frame writes are performed per-side (LEFT/RIGHT)
 *
 * @defgroup doly_lcdcontrol LcdControl
 * @brief Low-level LCD control API.
 * @{
 */

 /**
  * @brief LCD pixel color depth / bus format selection.
  */
enum class LcdColorDepth :uint32_t
{
	/** 12-bit color depth (packed). */
	L12BIT = 0x03,
	/** 18-bit color depth (packed). */
	L18BIT = 0x06,
};

/**
 * @brief Which LCD panel to address.
 */
enum LcdSide :uint8_t
{
	LEFT,
	RIGHT,
};

/**
 * @brief Frame descriptor used by writeLcd().
 */
struct LcdData
{
	/** Target LCD side. */
	LcdSide side;
	/** Pointer to the pixel buffer in the format returned by getColorDepth(). */
	uint8_t* buffer;
};

namespace LcdControl
{
	/**
	 * @brief Initialize the LCD device.
	 *
	 * @param depth LCD color depth to configure.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already initialized
	 * - -1 : open device failed
	 * - -2 : ioctl failed
	 */
	int8_t init(LcdColorDepth depth = LcdColorDepth::L12BIT);

	/**
	 * @brief Release resources and deinitialize the LCD device.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : already closed or not opened
	 */
	int8_t dispose();

	/**
	 * @brief Check whether the LCD subsystem is active (initialized).
	 * @return true if initialized; false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Fill a panel with a solid RGB color.
	 *
	 * @param side LCD side to fill.
	 * @param R Red component (0..255).
	 * @param G Green component (0..255).
	 * @param B Blue component (0..255).
	 */
	void LcdColorFill(LcdSide side, uint8_t R, uint8_t G, uint8_t B);

	/**
	 * @brief Write a buffer to the LCD.
	 *
	 * @param frame_data Frame descriptor containing target side and buffer pointer.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : ioctl error
	 * - -2 : not active (init() not called or failed)
	 *
	 * @warning @p frame_data and @p frame_data->buffer must be valid (non-null).
	 */
	int8_t writeLcd(LcdData* frame_data);

	/**
	 * @brief Get required buffer size in bytes for one full frame.
	 *
	 * Size depends on panel resolution and configured color depth.
	 * Example: 240*240*3 for 18-bit depth represented as 3 bytes/pixel.
	 *
	 * @return Buffer size in bytes.
	 */
	int getBufferSize();

	/**
	 * @brief Get the currently configured LCD color depth.
	 * @return Current color depth.
	 */
	LcdColorDepth getColorDepth();

	/**
	 * @brief Set LCD backlight brightness.
	 *
	 * @param value Brightness value (min = 0, max = 10).
	 * @return Status code (implementation-defined).
	 */
	int8_t setBrightness(uint8_t value);

	/**
	 * @brief Convert 24-bit (RGB) or 32-bit (RGBA) image data to the LCD buffer format.
	 *
	 * @param output Output buffer pointer (must have at least getBufferSize() bytes).
	 * @param input Input image data pointer (RGB or RGBA).
	 * @param input_RGBA Set true if @p input contains RGBA data (alpha is discarded).
	 *
	 * @warning @p output and @p input must be valid (non-null).
	 */
	void toLcdBuffer(uint8_t* output, uint8_t* input, bool input_RGBA = false);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();

};

/** @} */ // end of group doly_lcdcontrol
