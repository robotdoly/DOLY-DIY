#pragma once
#include <stdint.h>

/**
 * @file BatteryEventListener.h
 * @brief Public event types and listener interface for BatteryControl.
 *
 * This header defines:
 * - the BatteryStatus enum
 * - the BatteryEventListener interface used by BatteryEvent
 *
 * @ingroup doly_batterycontrol
 */

 /**
  * @defgroup doly_batterycontrol BatteryControl
  * @brief Doly battery monitoring SDK (C++).
  *
  * BatteryControl is exposed as a singleton-style API (namespace functions).
  * Events are delivered asynchronously through BatteryEvent / BatteryEventListener.
  * @{
  */

enum class BatteryStatus :uint8_t
{
	UNKNOWN, // this is defined on start to capture current state change
	CHARGING,
	DISCHARGING,
	FULL,
};

/**
 * @brief Interface for receiving battery events.
 *
 * Implement this interface and register it using BatteryEvent::AddListener().
 *
 * @note Default implementations are provided by the library implementation.
 * @warning Callbacks are typically invoked from an internal worker/event thread.
 *          Keep handlers fast and avoid blocking.
 * @warning The listener object must remain valid until it is removed via
 *          BatteryEvent::RemoveListener().
 */
class BatteryEventListener
{
public:
	virtual ~BatteryEventListener() = default;

	/**
	 * @brief Called when the battery capacity drops to or below the configured alarm threshold.
	 *
	 * @param capacity Current battery capacity in percent.
	 */
	virtual void onBatteryAlarm(uint8_t capacity);
	virtual void onBatteryStatus(BatteryStatus status);

};

/** @} */ // end of group doly_batterycontrol