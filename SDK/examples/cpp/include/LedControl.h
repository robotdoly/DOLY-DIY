#pragma once

#include "LedEvent.h"

/**
 * @file LedControl.h
 * @brief Public API for Doly LED control.
 *
 * This API controls the left/right arm LEDs and processes LED activities (color fades) using
 * an internal worker thread. Completion and error events can be observed via LedEvent.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - init() must be called before processing activities
 * - processActivity() is non-blocking (handled by a worker thread)
 *
 * @ingroup doly_ledcontrol
 */

namespace LedControl
{
	/**
	 * @brief Initialize the LED subsystem.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : already initialized
	 * - -1 : R Left GPIO init failed
	 * - -2 : G Left GPIO init failed
	 * - -3 : B Left GPIO init failed
	 * - -4 : R Right GPIO init failed
	 * - -5 : G Right GPIO init failed
	 * - -6 : B Right GPIO init failed
	 */
	int8_t init();

	/**
	 * @brief Stop the worker thread and release resources.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - -1..-6 : GPIO pin release failed.
	 */
	int8_t dispose();

	/**
	 * @brief Check whether the LED subsystem is active (initialized).
	 * @return true if initialized; false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Abort any running activity on the given side.
	 * @param side LED side to abort.
	 */
	void Abort(LedSide side);

	/**
	 * @brief Set a LED activity to be processed asynchronously.
	 *
	 * This is a non-blocking operation. The internal worker processes the activity and
	 * completion/error is reported via LedEvent callbacks.
	 *
	 * @param id User-defined activity identifier (forwarded to completion/error events).
	 * @param side LED side to run the activity on.
	 * @param activity Activity parameters (colors, fade time).
	 */
	void processActivity(uint16_t id, LedSide side, LedActivity activity);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();

};
