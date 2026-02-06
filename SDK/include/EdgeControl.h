#pragma once
#include <vector>
#include <cstdint>
#include "EdgeEvent.h"

/**
 * @file EdgeControl.h
 * @brief Public API for Doly edge/gap detection control (IR sensors).
 *
 * EdgeControl provides access to IR edge sensors and gap detection.
 * Typical update rate noted in the original header: ~21 Hz.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Works only after successful init()
 * - Events are delivered via EdgeEvent / EdgeEventListener
 *
 * Threading notes:
 * - Events are reported asynchronously (often from a worker thread)
 *
 * @ingroup doly_edgecontrol
 */

namespace EdgeControl
{
	/**
	 * @brief Initialize edge sensor control.
	 *
	 * Must be called before enabling control or reading sensors.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - <0 : error (implementation-defined)
	 */
	int8_t init();

	/**
	 * @brief Dispose edge sensor control and release resources.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : not initialized
	 * - -1..-6 : GPIO release failed (specific code indicates which GPIO failed)
	 */
	int8_t dispose();

	/**
	 * @brief Check whether edge sensor control is initialized and active.
	 *
	 * @return true if active, false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Start the sensor listening thread.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already running
	 * - -1 : not initialized
	 * 
	 * @note The initialize function enables the module on startup.
	 */
	int8_t enableControl();

	/**
	 * @brief Stop the sensor listening thread.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : not running
	 * - -1 : not initialized
	 */
	int8_t disableControl();

	/**
	 * @brief Get all IR sensor values.
	 *
	 * @return Vector containing the current state of all IR sensors.
	 */
	std::vector<IrSensor> getSensors();

	/**
	 * @brief Get IR sensors filtered by GPIO state.
	 *
	 * @param state Desired GPIO state to filter by.
	 *              0 = no ground detected (free)
	 *              1 = ground detected
	 *
	 * @return Vector containing sensors matching the specified state.
	 */
	std::vector<IrSensor> getSensors(GpioState state);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();

} // namespace EdgeControl