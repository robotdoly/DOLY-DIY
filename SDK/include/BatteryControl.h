#pragma once
#include "BatteryEvent.h"

/**
 * @file BatteryControl.h
 * @brief Public API for Doly battery monitoring and alarm control.
 *
 * BatteryControl provides access to:
 * - battery activity state
 * - current capacity percentage
 * - current battery status
 * - battery alarm threshold configuration
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Works only after successful init()
 * - Events are delivered via BatteryEvent / BatteryEventListener
 *
 * Threading notes:
 * - Battery monitoring performed asynchronously by the library
 * - Event callbacks are typically reported from an internal worker thread
 *
 * @ingroup doly_batterycontrol
 */

namespace BatteryControl
{
	/**
	 * @brief Check whether BatteryControl is initialized and active.
	 *
	 * @return true if active, false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Initialize battery monitoring/control.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : already active
	 */
	int8_t init();

	/**
	 * @brief Dispose battery monitoring/control and release resources.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : not initialized
	 */
	int8_t dispose();

	/**
	 * @brief Set battery alarm threshold.
	 *
	 * When battery capacity drops to or below this value, an alarm event may be fired.
	 * The repeat interval for the alarm is 10 seconds.
	 *
	 * @param value Battery capacity threshold in percent.
	 */
	void SetAlarmThreshold(uint8_t value);

	/**
	 * @brief Get current battery capacity.
	 *
	 * @return Battery capacity in percent.
	 */
	uint8_t getCapacity();

	/**
	 * @brief Get current library version.
	 *
	 * @return Version as float.
	 */
	float getVersion();


};