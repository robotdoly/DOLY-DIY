#pragma once
#include "GpioPin.h"

/**
 * @file IoControl.h
 * @brief Public API for Doly IO control (IO Port GPIO pin read/write).
 *
 * This header provides a small, single-file API for controlling Doly's IO port pins.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Uses the shared GPIO types from GpioPin.h (e.g., GpioState)
 *
 * @defgroup doly_iocontrol IoControl
 * @brief Doly IO control (GPIO pin read/write) API.
 * @{
 */

namespace IoControl
{
	/**
	 * @brief Write a GPIO state to an IO port pin.
	 *
	 * @param id User-defined identifier used for internal tracking/logging (can be any value).
	 * @param io_pin IO port pin number (valid range: 0..5).
	 * @param state Desired GPIO output state.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : invalid GPIO pin (must be IO port pin number 0..5)
	 * - -2 : GPIO write error
	 */
	int8_t writePin(uint16_t id, uint8_t io_pin, GpioState state);

	/**
	 * @brief Read the current GPIO state of an IO port pin.
	 *
	 * @param id User-defined identifier used for internal tracking/logging (can be any value).
	 * @param io_pin IO port pin number (valid range: 0..5).
	 *
	 * @return Current GPIO state. If @p io_pin is invalid or a read error occurs,
	 * the returned value is implementation-defined (see underlying GPIO layer).
	 */
	GpioState readPin(uint16_t id, uint8_t io_pin);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();
};

/** @} */ // end of group doly_iocontrol
