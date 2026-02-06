#pragma once
#include "TouchEventListener.h"

/**
 * @file TouchEvent.h
 * @brief Event registration helpers for the touch subsystem.
 *
 * Use this API to subscribe to touch state changes and touch activity notifications.
 * Two callback styles are supported:
 * - Object listeners (TouchEventListener*)
 * - Static/free function callbacks (function pointers)
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 *
 * @ingroup doly_touchcontrol
 */

namespace TouchEvent
{
	/**
	 * @brief Register a TouchEventListener instance.
	 *
	 * @param observer Listener object to register.
	 * @param priority If true, the listener is inserted with higher priority (called earlier).
	 *
	 * @warning Keep @p observer alive while it is registered. Unregister it before destruction.
	 */
	void AddListener(TouchEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a previously registered TouchEventListener instance.
	 * @param observer Listener object to unregister.
	 */
	void RemoveListener(TouchEventListener* observer);

	/**
	 * @brief Register a static/free function to receive touch state events.
	 * @param onEvent Callback invoked with side and state.
	 */
	void AddListenerOnTouch(void(*onEvent)(TouchSide side, TouchState state));

	/**
	 * @brief Unregister a previously registered touch callback.
	 * @param onEvent Callback to remove.
	 */
	void RemoveListenerOnTouch(void(*onEvent)(TouchSide side, TouchState state));

	/**
	 * @brief Register a static/free function to receive touch activity events.
	 * @param onActivityEvent Callback invoked with side and activity.
	 */
	void  AddListenerOnTouchActivity(void(*onActivityEvent)(TouchSide side, TouchActivity activity));

	/**
	 * @brief Unregister a previously registered touch activity callback.
	 * @param onActivityEvent Callback to remove.
	 */
	void RemoveListenerOnTouchActivity(void(*onActivityEvent)(TouchSide side, TouchActivity activity));

};
