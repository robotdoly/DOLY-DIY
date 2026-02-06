#pragma once
#include "TofEventListener.h"

/**
 * @file TofEvent.h
 * @brief Event registration helpers for the ToF (Time of Flight) subsystem.
 *
 * Use this API to subscribe to ToF proximity gesture and proximity threshold notifications.
 * Two callback styles are supported:
 * - Object listeners (TofEventListener*)
 * - Static/free function callbacks (function pointers)
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 *
 * @ingroup doly_tofcontrol
 */

namespace TofEvent
{
	/**
	 * @brief Register a TofEventListener instance.
	 *
	 * @param observer Listener object to register.
	 * @param priority If true, the listener is inserted with higher priority (called earlier).
	 *
	 * @warning Keep @p observer alive while it is registered. Unregister it before destruction.
	 */
	void AddListener(TofEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a previously registered TofEventListener instance.
	 * @param observer Listener object to unregister.
	 */
	void RemoveListener(TofEventListener* observer);

	/**
	 * @brief Register a static/free function to receive proximity gesture events.
	 * @param onEvent Callback invoked with left/right gesture results.
	 */
	void AddListenerOnProximityGesture(void(*onEvent)(TofGesture left, TofGesture right));

	/**
	 * @brief Unregister a previously registered proximity gesture callback.
	 * @param onEvent Callback to remove.
	 */
	void RemoveListenerOnProximityGesture(void(*onEvent)(TofGesture left, TofGesture right));

	/**
	 * @brief Register a static/free function to receive proximity threshold events.
	 * @param onEvent Callback invoked with left/right ToF data when threshold triggers.
	 */
	void AddListenerOnProximityThreshold(void(*onEvent)(TofData left, TofData right));

	/**
	 * @brief Unregister a previously registered proximity threshold callback.
	 * @param onEvent Callback to remove.
	 */
	void RemoveListenerOnProximityThreshold(void(*onEvent)(TofData left, TofData right));
};
