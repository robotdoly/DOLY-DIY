#pragma once

#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fstream>
#include <sys/ioctl.h>
#include <thread>

/**
 * @file FanControl.h
 * @brief Public API for Doly fan control.
 *
 * This header provides a single-file API for initializing and controlling the fan
 * management module, including optional automatic fan control.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - init() must be called before setFanSpeed()
 *
 * @defgroup doly_fancontrol FanControl
 * @brief Doly fan control API.
 * @{
 */

namespace FanControl
{
	/**
	 * @brief Initialize the Fan Control module.
	 *
	 * @param auto_control If true, automatic temperature control will manage fan speed.
	 * 
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : already initialized
	 * - <0 : error (implementation-defined)
	 */
	int8_t init(bool auto_control);

	/**
	 * @brief Dispose the Fan Control module and release resources.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : not initialized
	 * - <0 : error
	 */
	int8_t dispose();

	/**
	 * @brief Set fan speed as a percentage.
	 *
	 * @param percentage Fan speed percentage (0..100).
	 *
	 * @return Status code:
	 * - <0 : error
	 * 
	 * @note If @c auto_control was enabled in init(), the automatic controller may override
	 * or adjust the fan speed over time.
	 */
	int8_t setFanSpeed(uint8_t percentage);

	/**
	 * @brief Check whether the Fan Control module is active (initialized).
	 * @return true if initialized; false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();
};

/** @} */ // end of group doly_fancontrol
