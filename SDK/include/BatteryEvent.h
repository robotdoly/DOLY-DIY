#pragma once
#include "BatteryEventListener.h"

/**
 * @file BatteryEvent.h
 * @brief Event registration functions for BatteryControl.
 *
 * This header exposes listener registration/removal for battery-related events.
 * It supports:
 * - Object listeners (BatteryEventListener)
 * - Optional static callback listeners (function pointers)
 *
 * @ingroup doly_batterycontrol
 */

namespace BatteryEvent
{
	/**
	 * @brief Register a BatteryEventListener instance.
	 *
	 * @param observer Listener object pointer.
	 * @param priority If true, the listener may be inserted with higher priority
	 *                 (implementation-defined ordering).
	 *
	 * @warning The observer must remain valid until removed via RemoveListener().
	 */
	void AddListener(BatteryEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a BatteryEventListener instance.
	 *
	 * @param observer Listener object pointer previously registered.
	 */
	void RemoveListener(BatteryEventListener* observer);

	/**
	 * @brief Register a static callback for battery alarm events.
	 *
	 * @param onEvent Function pointer invoked with current battery capacity in percent.
	 *
	 * @warning The callback is typically invoked from an internal worker/event thread.
	 */
	void AddListenerOnAlarm(void(*onEvent)(uint8_t capacity));

	/**
	 * @brief Unregister a static callback for battery alarm events.
	 *
	 * @param onEvent Function pointer previously registered.
	 */
	void RemoveListenerOnAlarm(void(*onEvent)(uint8_t capacity));

};