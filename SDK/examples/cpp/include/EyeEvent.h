#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
#include "EyeEventListener.h"

/**
 * @file EyeEvent.h
 * @brief Event registration API for EyeControl.
 *
 * Use this module to subscribe to EyeControl lifecycle events.
 * You can register either:
 * - an EyeEventListener object, or
 * - static C-style callback functions.
 *
 * @ingroup doly_eyecontrol
 */

namespace EyeEvent
{
	/**
	 * @brief Register an EyeEventListener to receive events.
	 *
	 * @param observer Listener instance.
	 * @param priority If true, listener is placed at the front of the dispatch list.
	 *
	 * @warning The listener object must remain valid until it is removed via RemoveListener().
	 */
	void AddListener(EyeEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a previously added EyeEventListener.
	 * @param observer Listener instance to remove.
	 */
	void RemoveListener(EyeEventListener* observer);

	/**
	 * @brief Register a static callback invoked when an eye action/animation starts.
	 * @param onStart Callback function pointer.
	 */
	void AddListenerOnStart(void(*onStart)(uint16_t));

	/**
	 * @brief Unregister a previously added start callback.
	 * @param onStart Callback function pointer to remove.
	 */
	void RemoveListenerOnStart(void(*onStart)(uint16_t));

	/**
	 * @brief Register a static callback invoked when an eye action/animation completes.
	 * @param onComplete Callback function pointer.
	 */
	void AddListenerOnComplete(void(*onComplete)(uint16_t));

	/**
	 * @brief Unregister a previously added complete callback.
	 * @param onComplete Callback function pointer to remove.
	 */
	void RemoveListenerOnComplete(void(*onComplete)(uint16_t));

	/**
	 * @brief Register a static callback invoked when an eye action/animation is aborted.
	 * @param onAbort Callback function pointer.
	 */
	void  AddListenerOnAbort(void(*onAbort)(uint16_t));

	/**
	 * @brief Unregister a previously added abort callback.
	 * @param onAbort Callback function pointer to remove.
	 */
	void  RemoveListenerOnAbort(void(*onAbort)(uint16_t));
} // namespace EyeEvent