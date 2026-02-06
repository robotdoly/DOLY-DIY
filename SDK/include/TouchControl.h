#pragma once
#include <stdint.h>
#include "TouchEvent.h"

/**
 * @file TouchControl.h
 * @brief Public API for Doly touch sensor control.
 *
 * This API provides a touch controller that samples touch sensors and reports events
 * asynchronously via TouchEvent callbacks.
 *
 * Overview:
 * - Single global touch controller (no instances).
 * - Must call init() before use; call dispose() to stop the controller.
 * - Events are delivered via TouchEvent and TouchEventListener.
 * - Typical sampling frequency is ~20 Hz (implemented by the internal worker thread).
 *
 * Threading notes:
 * - Events are reported asynchronously from an internal worker thread.
 *
 * @ingroup doly_touchcontrol
 */

namespace TouchControl
{
	/**
	 * @brief Initialize the touch controller and start the worker thread.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already initialized (no-op)
	 * - -1 : left sensor GPIO initialization failed
	 * - -2 : right sensor GPIO initialization failed
	 */
	int8_t init();

	/**
	 * @brief Stop the worker thread and release resources.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : left sensor GPIO release failed
	 * - -2 : right sensor GPIO release failed
	 */
	int8_t dispose();

	/**
	 * @brief Check whether the touch controller is active (initialized and running).
	 * @return true if active; false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Check whether the given side is currently touched.
	 * @param side Sensor side to query.
	 * @return true if touched; false otherwise.
	 */
	bool isTouched(TouchSide side);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();
}
