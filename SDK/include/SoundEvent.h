#pragma once
#include <stdint.h>
#include "SoundEventListener.h"

/**
 * @file SoundEvent.h
 * @brief Event registration helpers for the sound subsystem.
 *
 * Use this API to subscribe to sound playback notifications.
 * Two callback styles are supported:
 * - Object listeners (SoundEventListener*)
 * - Static/free function callbacks (function pointers)
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 *
 * @ingroup doly_soundcontrol
 */

namespace SoundEvent
{
	/**
	 * @brief Register a SoundEventListener instance.
	 *
	 * @param observer Listener object to register.
	 * @param priority If true, the listener is inserted with higher priority (called earlier).
	 *
	 * @warning Keep @p observer alive while it is registered. Unregister it before destruction.
	 */
	void AddListener(SoundEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a previously registered SoundEventListener instance.
	 * @param observer Listener object to unregister.
	 */
	void RemoveListener(SoundEventListener* observer);

	/**
	 * @brief Register a static/free function to receive begin events.
	 * @param onComplete Callback invoked when playback begins.
	 */
	void AddListenerOnBegin(void(*onComplete)(uint16_t id, float volume));

	/**
	 * @brief Unregister a previously registered begin callback.
	 * @param onComplete Callback to remove.
	 */
	void RemoveListenerOnBegin(void(*onComplete)(uint16_t id, float volume));

	/**
	 * @brief Register a static/free function to receive completion events.
	 * @param onComplete Callback invoked when playback completes.
	 */
	void AddListenerOnComplete(void(*onComplete)(uint16_t id));

	/**
	 * @brief Unregister a previously registered completion callback.
	 * @param onComplete Callback to remove.
	 */
	void RemoveListenerOnComplete(void(*onComplete)(uint16_t id));

	/**
	 * @brief Register a static/free function to receive abort events.
	 * @param onAbort Callback invoked when playback is aborted.
	 */
	void  AddListenerOnAbort(void(*onAbort)(uint16_t id));

	/**
	 * @brief Unregister a previously registered abort callback.
	 * @param onAbort Callback to remove.
	 */
	void RemoveListenerOnAbort(void(*onAbort)(uint16_t id));

	/**
	 * @brief Register a static/free function to receive error events.
	 * @param onAbort Callback invoked when a playback error occurs.
	 */
	void  AddListenerOnError(void(*onAbort)(uint16_t id));

	/**
	 * @brief Unregister a previously registered error callback.
	 * @param onAbort Callback to remove.
	 */
	void RemoveListenerOnError(void(*onAbort)(uint16_t id));

};
