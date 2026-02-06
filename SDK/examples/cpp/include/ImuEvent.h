#pragma once
#include "ImuEventListener.h"
#include <string>

/**
 * @file ImuEvent.h
 * @brief Event registration helpers for the IMU subsystem.
 *
 * Use this API to subscribe to IMU updates and gesture notifications.
 * Two callback styles are supported:
 * - Object listeners (ImuEventListener*)
 * - Static/free function callbacks (function pointers)
 *
 * Threading notes:
 * - Callbacks may be invoked from an internal worker/event thread.
 *
 * @ingroup doly_imucontrol
 */

namespace ImuEvent
{
	/**
	 * @brief Register an ImuEventListener instance.
	 *
	 * @param observer Listener object to register.
	 * @param priority If true, the listener is inserted with higher priority (called earlier).
	 *
	 * @warning Keep @p observer alive while it is registered. Unregister it before destruction.
	 */
	void AddListener(ImuEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a previously registered ImuEventListener instance.
	 * @param observer Listener object to unregister.
	 */
	void RemoveListener(ImuEventListener* observer);

	/**
	 * @brief Register a static/free function to receive IMU update events.
	 * @param Imu_callback Callback invoked with the latest IMU data.
	 */
	void AddListenerUpdateEvent(void(*Imu_callback)(ImuData data));

	/**
	 * @brief Unregister a previously registered IMU update callback.
	 * @param Imu_callback Callback to remove.
	 */
	void RemoveListenerUpdateEvent(void(*Imu_callback)(ImuData data));

	/**
	 * @brief Register a static/free function to receive gesture events.
	 * @param gesture_cb Callback invoked on gesture detection.
	 */
	void AddListenerGestureEvent(void(*gesture_cb)(ImuGesture type, GestureDirection from));

	/**
	 * @brief Unregister a previously registered gesture callback.
	 * @param gesture_cb Callback to remove.
	 */
	void RemoveListenerGestureEvent(void(*gesture_cb)(ImuGesture type, GestureDirection from));

	/**
	 * @brief Convert a gesture enum value to a human-readable string.
	 * @param type Gesture type.
	 * @return String representation of @p type.
	 */
	std::string getGestureStr(ImuGesture type);

	/**
	 * @brief Convert a direction enum value to a human-readable string.
	 * @param from Direction value.
	 * @return String representation of @p from.
	 */
	std::string getDirectionStr(GestureDirection from);
};
