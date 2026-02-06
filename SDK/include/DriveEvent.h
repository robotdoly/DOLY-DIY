#pragma once
#include "DriveEventListener.h"

/**
 * @file DriveEvent.h
 * @brief Event dispatcher API for the Doly DriveControl subsystem.
 *
 * DriveEvent delivers notifications to:
 * - registered DriveEventListener objects
 * - optional C-style function callbacks
 *
 * @ingroup doly_drivecontrol
 */

namespace DriveEvent
{
	/**
	 * @brief Register a listener object to receive drive events.
	 *
	 * @param observer Pointer to a listener instance (must not be null).
	 * @param priority If true, the listener is inserted with priority ordering (implementation-defined).
	 *
	 * @warning The listener object must remain valid until removed with RemoveListener().
	 * @note Callbacks are typically invoked from an internal worker/event thread.
	 */
	void AddListener(DriveEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a listener object.
	 * @param observer Pointer previously passed to AddListener().
	 */
	void RemoveListener(DriveEventListener* observer);

	/**
	 * @brief Register a C-style callback for “command complete” events.
	 * @param onComplete Function pointer called with (id).
	 *
	 * @warning The callback is invoked asynchronously (often from a worker/event thread).
	 */
	void AddListenerOnComplete(void(*onComplete)(uint16_t id));

	/**
	 * @brief Remove a previously registered “command complete” callback.
	 * @param onComplete Same function pointer passed to AddListenerOnComplete().
	 */
	void RemoveListenerOnComplete(void(*onComplete)(uint16_t id));

	/**
	 * @brief Register a C-style callback for error events.
	 * @param onError Function pointer called with (id, side, type).
	 *
	 * @warning The callback is invoked asynchronously (often from a worker/event thread).
	 */
	void AddListenerOnError(void(*onError)(uint16_t id, DriveMotorSide side, DriveErrorType type));

	/**
	 * @brief Remove a previously registered error callback.
	 * @param onError Same function pointer passed to AddListenerOnError().
	 */
	void RemoveListenerOnError(void(*onError)(uint16_t id, DriveMotorSide side, DriveErrorType type));

	/**
	 * @brief Register a C-style callback for drive state changes.
	 * @param onChange Function pointer called with (drive_type, state).
	 *
	 * @warning The callback is invoked asynchronously (often from a worker/event thread).
	 */
	void AddListenerOnStateChange(void(*onChange)(DriveType drive_type, DriveState state));

	/**
	 * @brief Remove a previously registered state-change callback.
	 * @param onChange Same function pointer passed to AddListenerOnStateChange().
	 */
	void RemoveListenerOnStateChange(void(*onChange)(DriveType drive_type, DriveState state));

} // namespace DriveEvent