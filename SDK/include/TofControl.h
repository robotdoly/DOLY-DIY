#pragma once
#include <stdint.h>
#include <vector>
#include "TofEvent.h"

/**
 * @file TofControl.h
 * @brief Public API for Doly ToF (Time of Flight) sensor control.
 *
 * This API manages left/right ToF sensors, supports single-shot reads, and can run a
 * continuous reading loop for gesture detection and proximity threshold notifications.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - init() must be called before continuous operations
 * - setup_continuous() enables frequent readings handled internally; events are reported via TofEvent
 *
 * @ingroup doly_tofcontrol
 */

namespace TofControl
{
	/**
	 * @brief Initialize ToF sensors.
	 *
	 * @param offset_left Offset to apply to left sensor readings.
	 * @param offset_right Offset to apply to right sensor readings.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already running
	 * - -1 : both sensor init failed
	 * - -2 : left sensor init failed
	 * - -3 : right sensor init failed
	 */
	int8_t init(int8_t offset_left = 0, int8_t offset_right = 0);

	/**
	 * @brief Dispose/stop the ToF subsystem and release resources.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : not initialized
	 */
	int8_t dispose();

	/**
	 * @brief Configure continuous reading for gesture detection and threshold events.
	 *
	 * @param interval_ms Read interval in milliseconds (max = 2550 ms). Use 0 to stop continuous reading.
	 * @param distance Proximity threshold distance in millimeters:
	 * - 0 disables proximity threshold events
	 * - 120 mm is the maximum supported threshold
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already running
	 * - -1 : interval_ms out of range (max = 2550)
	 * - -2 : ToF devices not ready
	 * - -3 : proximity distance out of range (0..120 mm)
	 *
	 * @note For better gesture detection, recommended interval is 30..80 ms (ideal: 50 ms).
	 */
	int8_t setup_continuous(uint16_t interval_ms = 50, uint8_t distance = 0);

	/**
	 * @brief Read both sensors once and return the latest values.
	 *
	 * Continuous setup is not required for single reads.
	 *
	 * @return Vector containing left and right sensor data.
	 */
	std::vector<TofData> getSensorsData();

	/**
	 * @brief Check whether the ToF subsystem is active (initialized).
	 * @return true if initialized; false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Check whether the subsystem is currently performing frequent/continuous readings.
	 * @return true if continuous reading is active; false otherwise.
	 */
	bool isReading();

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();
};
