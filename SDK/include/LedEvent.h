#pragma once
#include <stdint.h>

#include "LedEventListener.h"

/**
 * @file LedEvent.h
 * @brief Event registration helpers for the LED subsystem.
 *
 * Use this API to subscribe to LED completion and error notifications.
 * Two callback styles are supported:
 * - Object listeners (LedEventListener*)
 * - Static/free function callbacks (function pointers)
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 *
 * @ingroup doly_ledcontrol
 */

namespace LedEvent
{
	/**
	 * @brief Register a LedEventListener instance.
	 *
	 * @param observer Listener object to register.
	 * @param priority If true, the listener is inserted with higher priority (called earlier).
	 *
	 * @warning Keep @p observer alive while it is registered. Unregister it before destruction.
	 */
	void AddListener(LedEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a previously registered LedEventListener instance.
	 * @param observer Listener object to unregister.
	 */
	void RemoveListener(LedEventListener* observer);

	/**
	 * @brief Register a static/free function to receive completion events.
	 *
	 * @param onEvent Callback invoked when an activity completes.
	 */
	void AddListenerOnComplete(void(*onEvent)(uint16_t id, LedSide side));

	/**
	 * @brief Unregister a previously registered completion callback.
	 * @param onEvent Callback to remove.
	 */
	void RemoveListenerOnComplete(void(*onEvent)(uint16_t id, LedSide side));

	/**
	 * @brief Register a static/free function to receive error events.
	 *
	 * @param onError Callback invoked when an activity fails or is aborted.
	 */
	void AddListenerOnError(void(*onError)(uint16_t id, LedSide side, LedErrorType type));

	/**
	 * @brief Unregister a previously registered error callback.
	 * @param onError Callback to remove.
	 */
	void RemoveListenerOnError(void(*onError)(uint16_t id, LedSide side, LedErrorType type));

};
