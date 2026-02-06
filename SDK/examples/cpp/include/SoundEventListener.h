#pragma once
#include <stdint.h>

/**
 * @file SoundEventListener.h
 * @brief Listener interface for sound playback events.
 *
 * This header defines the SoundEventListener callback interface used by SoundEvent.
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 * - Keep listener objects alive while they are registered (see SoundEvent::AddListener()).
 *
 * @defgroup doly_soundcontrol SoundControl
 * @brief Doly sound playback control and event API.
 * @{
 */

 /**
  * @brief Observer interface for receiving sound playback events.
  *
  * Register an implementation using SoundEvent::AddListener().
  *
  * @warning Do not destroy a listener object while it is registered.
  */
class SoundEventListener
{
public:
	virtual ~SoundEventListener() = default;

	/**
	 * @brief Called when playback begins.
	 * @param id User-defined sound id passed to SoundControl::play().
	 * @param volume Current playback volume (implementation-defined scale).
	 */
	virtual void onSoundBegin(uint16_t id, float volume);

	/**
	 * @brief Called when playback completes successfully.
	 * @param id User-defined sound id passed to SoundControl::play().
	 */
	virtual void onSoundComplete(uint16_t id);

	/**
	 * @brief Called when playback is aborted.
	 * @param id User-defined sound id passed to SoundControl::play().
	 */
	virtual void onSoundAbort(uint16_t id);

	/**
	 * @brief Called when a playback error occurs.
	 * @param id User-defined sound id passed to SoundControl::play().
	 */
	virtual void onSoundError(uint16_t id);
};

/** @} */ // end of group doly_soundcontrol
